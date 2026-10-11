import idautils
import idc
import ida_funcs
import ida_gdl
import ida_ua
import ida_bytes
import ida_segment
import re

MIN_BLOB_LEN = 8        # минимальная длина блока для вывода
MAX_BLOB_LEN = 0x1000   # ограничение на длину блока
SKIP_STRLIT = False      # True - пропускать то, что IDA уже определила как строку
NAME_PREFIXES = ("unk_", "byte_")   # брать только блоки с такими авто-именами
                                    # (None - не фильтровать по имени)
ONLY_READONLY = False    # только неизменяемые секции (.rdata), без .data/.bss
MIN_UNIQUE_BYTES = 6    # минимум разных байт (отсекает 00 00.. / FF FF.. и т.п.)

PEB_OFFSETS = {
    ("fs", 0x30): "PEB (Windows x86)",
    ("gs", 0x60): "PEB (Windows x64)",
}

TIME_API_NAMES = {
    "gettickcount": "GetTickCount",
    "gettickcount64": "GetTickCount64",
    "queryperformancecounter": "QueryPerformanceCounter",
    "queryperformancefrequency": "QueryPerformanceFrequency",
    "getsystemtime": "GetSystemTime",
    "getsystemtimeasfiletime": "GetSystemTimeAsFileTime",
    "getsystemtimepreciseasfiletime": "GetSystemTimePreciseAsFileTime",
    "timegettime": "timeGetTime",
    "ntquerysystemtime": "NtQuerySystemTime",
    "ntqueryperformancecounter": "NtQueryPerformanceCounter",
    "zwquerysystemtime": "ZwQuerySystemTime",
    "clock_gettime": "clock_gettime",
    "time": "time",
    "clock": "clock",
}

TIME_INSTRUCTIONS = {
    "rdtsc": "RDTSC",
    "rdtscp": "RDTSCP",
}


RARE_INSTRUCTIONS = {
    "int": "Software interrupt",
    "int1": "ICEBP / INT1",
    "int3": "Breakpoint interrupt",
    "into": "Overflow interrupt (x86)",
    "icebp": "ICEBP",
    "ud0": "Undefined instruction",
    "ud1": "Undefined instruction",
    "ud2": "Undefined instruction",
    "hlt": "Halt",
    "cli": "Disable interrupts",
    "sti": "Enable interrupts",
    "in": "Port input",
    "out": "Port output",
    "ins": "String port input",
    "outs": "String port output",
    "sidt": "Store IDT",
    "sgdt": "Store GDT",
    "sldt": "Store LDT",
    "str": "Store task register",
    "smsw": "Store machine status word",
    "lldt": "Load LDT",
    "ltr": "Load task register",
    "lgdt": "Load GDT",
    "lidt": "Load IDT",
    "invlpg": "Invalidate TLB page",
    "invd": "Invalidate cache",
    "wbinvd": "Write back and invalidate cache",
    "syscall": "System call",
    "sysret": "System return",
    "sysenter": "Fast system call",
    "sysexit": "Fast system return",
    "vmcall": "Virtual machine call",
    "vmmcall": "Virtual machine call",
    "vmrun": "Enter VMX guest",
    "vmlaunch": "Launch VMX guest",
    "vmresume": "Resume VMX guest",
    "vmxoff": "Leave VMX operation",
    "xbegin": "Transactional execution",
    "xend": "Transactional execution end",
    "xtest": "Transactional execution test",
    "lar": "Load access rights",
    "lsl": "Load segment limit",
    "verr": "Verify segment for reading",
    "verw": "Verify segment for writing",
    "arpl": "Adjust RPL (x86)",
    "bound": "Bounds check (x86)",
    "aam": "ASCII adjust after multiply",
    "aad": "ASCII adjust before divide",
    "daa": "Decimal adjust after addition",
    "das": "Decimal adjust after subtraction",
    "aaa": "ASCII adjust after addition",
    "aas": "ASCII adjust after subtraction",
    "rsm": "Resume from SMM",
    "rdmsr": "Read model-specific register",
    "wrmsr": "Write model-specific register",
    "rdpmc": "Read performance counter",
    "xgetbv": "Read extended control register",
    "xsetbv": "Write extended control register",
    "cpuid": "CPU identification",
}

SEGMENT_REGISTERS = {
    "cs", "ds", "es", "ss"
}


PROTECTION_APIS = {
    "IsDebuggerPresent":
        "Проверка наличия отладчика",

    "CheckRemoteDebuggerPresent":
        "Проверка удалённого отладчика",

    "NtQueryInformationProcess":
        "Получение информации о процессе",

    "OutputDebugStringA":
        "Взаимодействие с отладчиком",

    "OutputDebugStringW":
        "Взаимодействие с отладчиком",

    "SetUnhandledExceptionFilter":
        "Установка обработчика исключений",

    "AddVectoredExceptionHandler":
        "Регистрация обработчика исключений",

    "RemoveVectoredExceptionHandler":
        "Удаление обработчика исключений",

    "RaiseException":
        "Генерация исключения",

    "VirtualProtect":
        "Изменение защиты памяти",

    "VirtualProtectEx":
        "Изменение защиты памяти другого процесса",

    "VirtualAlloc":
        "Выделение памяти",

    "VirtualAllocEx":
        "Выделение памяти другого процесса",

    "NtProtectVirtualMemory":
        "Изменение защиты памяти через NT API",

    "NtAllocateVirtualMemory":
        "Выделение памяти через NT API",

    "FlushInstructionCache":
        "Сброс кэша инструкций",

    "GetModuleHandleA":
        "Получение дескриптора модуля",

    "GetModuleHandleW":
        "Получение дескриптора модуля",

    "GetProcAddress":
        "Динамическое разрешение API",

    "LoadLibraryA":
        "Динамическая загрузка библиотеки",

    "LoadLibraryW":
        "Динамическая загрузка библиотеки",
}


VM_MARKERS = (
    "vmware",
    "virtualbox",
    "vbox",
    "qemu",
    "xen",
    "hyper-v",
    "hypervisor",
)

def is_library(f):
    return bool(f.flags & (ida_funcs.FUNC_LIB | ida_funcs.FUNC_THUNK))


def get_loop_blocks(func):
    """Возвращает список циклов (каждый цикл - множество блоков)."""
    fc = ida_gdl.FlowChart(func, flags=ida_gdl.FC_PREDS)
    loops = []
    for tail in fc:
        for head in tail.succs():
            if head.start_ea <= tail.start_ea:  # обратное ребро
                body = {head.id: head, tail.id: tail}
                stack = [tail]

                while stack:
                    b = stack.pop()

                    if b.id == head.id:
                        continue

                    for p in b.preds():
                        if p.id not in body and p.start_ea >= head.start_ea:
                            body[p.id] = p
                            stack.append(p)

                loops.append(list(body.values()))

    return loops


def is_useful_xor(ea):
    """xor, но не обнуление регистра (xor eax, eax)."""
    if idc.print_insn_mnem(ea) != "xor":
        return False

    insn = ida_ua.insn_t()

    if ida_ua.decode_insn(insn, ea) == 0:
        return False

    a, b = insn.ops[0], insn.ops[1]
    if a.type == ida_ua.o_reg and b.type == ida_ua.o_reg and a.reg == b.reg:
        return False

    return True


def find_xor_crypto():
    funcs_xor = {}

    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)
        if not f or is_library(f):
            continue

        for loop in get_loop_blocks(f):
            for blk in loop:
                for ea in idautils.Heads(blk.start_ea, blk.end_ea):
                    if is_useful_xor(ea):
                        funcs_xor.setdefault(fea, set()).add(ea)

    return {fea: sorted(eas) for fea, eas in funcs_xor.items()}

def find_byte_strings():
    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)
        if not f or is_library(f):
            continue

        chain = []

        for ea in idautils.Heads(f.start_ea, f.end_ea):
            b = _get_printable_byte(ea)
            if b is not None:
                chain.append((ea, b))
                continue

            if len(chain) >= 4:
                _print_chain(f, chain)
            chain = []

        if len(chain) >= 4:
            _print_chain(f, chain)


def _get_printable_byte(ea):
    if idc.print_insn_mnem(ea) != "mov":
        return None

    insn = ida_ua.insn_t()
    if ida_ua.decode_insn(insn, ea) == 0:
        return None
    if len(insn.ops) < 2:
        return None

    dst, src = insn.ops[0], insn.ops[1]

    if dst.type != ida_ua.o_displ:
        return None
    if src.type != ida_ua.o_imm:
        return None

    v = src.value & 0xFF
    if not (32 <= v < 127):
        return None

    return v


def _print_chain(f, chain):
    text = "".join(chr(b) for _, b in chain)
    print("[byte-str] func 0x%X  len=%d  %r"
          % (f.start_ea, len(chain), text))


def _is_data_target(ea):
    """Адрес лежит в неисполняемом сегменте и не размечен как код."""
    seg = ida_segment.getseg(ea)
    if not seg or (seg.perm & ida_segment.SEGPERM_EXEC):
        return False
    return not ida_bytes.is_code(ida_bytes.get_flags(ea))


def read_blob(start):
    """Читает байты от метки start до следующей метки/инструкции/конца сегмента."""
    seg = ida_segment.getseg(start)
    if not seg:
        return b""

    data = bytearray()
    cur = start

    while cur < seg.end_ea and len(data) < MAX_BLOB_LEN:
        flags = ida_bytes.get_flags(cur)

        if ida_bytes.is_code(flags):
            break

        # следующая метка или объект с xref - граница блока
        if cur != start and (ida_bytes.has_any_name(flags) or ida_bytes.has_xref(flags)):
            break

        size = ida_bytes.get_item_size(cur) or 1
        chunk = ida_bytes.get_bytes(cur, size)
        if chunk is None:
            break

        data += chunk
        cur += size

    return bytes(data)


def is_interesting_blob(ea, data):
    """Отсев мусора: импорты, заливка, указатели, структуры, готовые строки."""
    flags = ida_bytes.get_flags(ea)

    if NAME_PREFIXES:
        if not idc.get_name(ea).startswith(NAME_PREFIXES):
            return False

    if ONLY_READONLY:
        seg = ida_segment.getseg(ea)
        if seg and (seg.perm & ida_segment.SEGPERM_WRITE):
            return False

    if SKIP_STRLIT and ida_bytes.is_strlit(flags):
        return False
    if ida_bytes.is_off0(flags) or ida_bytes.is_struct(flags):
        return False

    if len(set(data)) < MIN_UNIQUE_BYTES:
        return False

    return True


def find_data_blobs():
    """Ищет блоки данных (unk_XXXX и т.п.), на которые ссылаются небиблиотечные функции."""
    blobs = {}

    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)
        if not f or is_library(f):
            continue

        for head in idautils.Heads(f.start_ea, f.end_ea):
            if not ida_bytes.is_code(ida_bytes.get_flags(head)):
                continue

            for xr in idautils.XrefsFrom(head, 0):
                if xr.iscode:
                    continue
                if not _is_data_target(xr.to):
                    continue
                blobs.setdefault(xr.to, []).append((fea, head))

    for ea in sorted(blobs):
        data = read_blob(ea)
        if len(data) < MIN_BLOB_LEN:
            continue
        if not is_interesting_blob(ea, data):
            continue

        ascii_repr = "".join(chr(b) if 32 <= b < 127 else "." for b in data)
        print("[blob] 0x%X  %s  len=%d" % (ea, idc.get_name(ea), len(data)))
        print("       hex  : %s" % " ".join("%02X" % b for b in data))
        print("       ascii: %s" % ascii_repr)

        for fea, xea in blobs[ea]:
            print("       xref : %s (0x%X) <- 0x%X"
                  % (idc.get_func_name(fea), fea, xea))

def find_peb_accesses():
    results = {}

    pattern = re.compile(
        r"\b(fs|gs)\s*:\s*"
        r"(?:\[\s*)?"
        r"(0x[0-9a-f]+|[0-9a-f]+h?)\b",
        re.IGNORECASE
    )

    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)

        if not f or is_library(f):
            continue

        for ea in idautils.Heads(f.start_ea, f.end_ea):
            if not ida_bytes.is_code(ida_bytes.get_flags(ea)):
                continue

            disasm = idc.generate_disasm_line(ea, 0)
            if not disasm:
                continue

            matches = list(pattern.finditer(disasm))
            if not matches:
                continue

            descriptions = []

            for match in matches:
                segment = match.group(1).lower()
                raw_offset = match.group(2).lower()

                if raw_offset.startswith("0x"):
                    offset = int(raw_offset, 16)
                elif raw_offset.endswith("h"):
                    offset = int(raw_offset[:-1], 16)
                else:
                    offset = int(raw_offset, 16)

                desc = PEB_OFFSETS.get((segment, offset))

                if desc:
                    descriptions.append(desc)
                else:
                    descriptions.append(
                        "FS/GS access: %s:[0x%X]"
                        % (segment.upper(), offset)
                    )

            results.setdefault(fea, []).append(
                (ea, "; ".join(descriptions), disasm)
            )

    return results


def print_peb_accesses():
    results = find_peb_accesses()

    total = 0

    for fea in sorted(results):
        accesses = results[fea]

        print(
            "\n[+] %s | 0x%X"
            % (idc.get_func_name(fea), fea)
        )

        for ea, description, disasm in accesses:
            print(
                "    0x%X  [%s]  %s"
                % (ea, description, disasm)
            )
            total += 1

    print("\nНайдено обращений: %d" % total)

    return results

def find_time_checks():
    results = {}

    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)

        if not f or is_library(f):
            continue

        for ea in idautils.Heads(f.start_ea, f.end_ea):
            if not ida_bytes.is_code(ida_bytes.get_flags(ea)):
                continue

            mnem = idc.print_insn_mnem(ea).lower()
            disasm = idc.generate_disasm_line(ea, 0) or idc.GetDisasm(ea)

            category = None
            description = None

            if mnem in TIME_INSTRUCTIONS:
                category = "CPU counter"
                description = TIME_INSTRUCTIONS[mnem]

            elif mnem in ("call", "callq", "bl", "blr"):
                target_name = idc.get_name(
                    idc.get_operand_value(ea, 0)
                ).lower()

                normalized = target_name.split(".")[-1]
                normalized = normalized.lstrip("_")
                normalized = normalized.split("@")[0]

                if normalized in TIME_API_NAMES:
                    category = "Time API"
                    description = TIME_API_NAMES[normalized]

                else:
                    text = disasm.lower()

                    for api_name, canonical_name in TIME_API_NAMES.items():
                        if re.search(
                            r"(?<![a-z0-9_])"
                            + re.escape(api_name)
                            + r"(?![a-z0-9_])",
                            text
                        ):
                            category = "Time API"
                            description = canonical_name
                            break

            if category:
                results.setdefault(fea, []).append(
                    (ea, category, description, disasm)
                )

    return results


def print_time_checks():
    results = find_time_checks()
    total = 0

    for fea in sorted(results):
        print(
            "\n[+] %s | 0x%X"
            % (idc.get_func_name(fea), fea)
        )

        for ea, category, description, disasm in results[fea]:
            print(
                "    0x%X  [%s] %s | %s"
                % (ea, category, description, disasm)
            )
            total += 1

    print("\nНайдено мест: %d" % total)

    return results

def find_rare_instructions():
    results = {}

    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)

        if not f or is_library(f):
            continue

        for ea in idautils.Heads(f.start_ea, f.end_ea):
            if not ida_bytes.is_code(ida_bytes.get_flags(ea)):
                continue

            mnem = idc.print_insn_mnem(ea).lower()
            disasm = idc.generate_disasm_line(ea, 0) or idc.GetDisasm(ea)

            category = None
            description = None

            if mnem in RARE_INSTRUCTIONS:
                category = "Rare instruction"
                description = RARE_INSTRUCTIONS[mnem]

                if mnem == "int":
                    insn = ida_ua.insn_t()

                    if ida_ua.decode_insn(insn, ea):
                        op = insn.ops[0]

                        if op.type == ida_ua.o_imm:
                            vector = op.value

                            if vector == 0x2D:
                                description = "INT 2D (anti-debug candidate)"
                            elif vector == 0x03:
                                description = "INT 3 (breakpoint)"
                            elif vector == 0x01:
                                description = "INT 1 (debug exception)"
                            else:
                                description = "INT 0x%X" % vector

            segment_hits = []

            for reg in SEGMENT_REGISTERS:
                if re.search(
                    r"(?<![a-z0-9_])" + reg + r"(?![a-z0-9_])",
                    disasm.lower()
                ):
                    segment_hits.append(reg.upper())

            if segment_hits:
                seg_desc = "Segment register: " + ", ".join(
                    sorted(set(segment_hits))
                )

                if category:
                    description += "; " + seg_desc
                else:
                    category = "Segment register"
                    description = seg_desc

            if category:
                results.setdefault(fea, []).append(
                    (ea, category, description, disasm)
                )

    return results


def print_rare_instructions():
    results = find_rare_instructions()
    total = 0

    for fea in sorted(results):
        print(
            "\n[+] %s | 0x%X"
            % (idc.get_func_name(fea), fea)
        )

        for ea, category, description, disasm in results[fea]:
            print(
                "    0x%X  [%s] %s | %s"
                % (ea, category, description, disasm)
            )
            total += 1

    print("\nНайдено инструкций и обращений: %d" % total)

    return results

def find_protection_indicators():
    results = {}

    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)

        if not f or is_library(f):
            continue

        for ea in idautils.Heads(f.start_ea, f.end_ea):
            if not ida_bytes.is_code(ida_bytes.get_flags(ea)):
                continue

            mnem = idc.print_insn_mnem(ea).lower()
            disasm = (
                idc.generate_disasm_line(ea, 0)
                or idc.GetDisasm(ea)
            )

            text = disasm.lower()
            found = []

            if mnem in ("call", "callq", "jmp"):
                for api, description in PROTECTION_APIS.items():
                    if re.search(
                        r"(?<![a-z0-9_])"
                        + re.escape(api.lower())
                        + r"(?![a-z0-9_])",
                        text
                    ):
                        found.append((
                            "Protection API",
                            api + ": " + description
                        ))

            if mnem == "cpuid":
                found.append((
                    "Environment check",
                    "CPUID: проверка характеристик CPU"
                ))

            for marker in VM_MARKERS:
                if marker in text:
                    found.append((
                        "VM marker",
                        "Маркер виртуальной среды: " + marker
                    ))

            if found:
                entries = results.setdefault(fea, [])

                for category, description in found:
                    item = (ea, category, description, disasm)

                    if item not in entries:
                        entries.append(item)

    return results


def print_protection_indicators():
    results = find_protection_indicators()
    total = 0

    for fea in sorted(results):
        print(
            "\n[+] %s | 0x%X"
            % (idc.get_func_name(fea), fea)
        )

        for ea, category, description, disasm in results[fea]:
            print(
                "    0x%X  [%s] %s | %s"
                % (ea, category, description, disasm)
            )
            total += 1

    print("\nНайдено совпадений: %d" % total)

    print("\nПоиск строк виртуальных сред")

    string_count = 0

    for s in idautils.Strings():
        value = str(s).lower()

        for marker in VM_MARKERS:
            if marker in value:
                print(
                    "    0x%X  [VM string] %s"
                    % (s.ea, str(s))
                )
                string_count += 1
                break

    print("Найдено строк: %d" % string_count)

    return results

print("Поиск XOR")

funcs_xor = find_xor_crypto()
for fea in funcs_xor.keys():
    print("[+] %s | 0x%X" % (idc.get_func_name(fea), fea))

    for ea in funcs_xor[fea]:
        print("      0x%X  %s" % (ea, idc.GetDisasm(ea)))

print("\nПоиск XOR-строк")

find_byte_strings()

print("\nПоиск блоков данных (unk_*)")

find_data_blobs()

print("\nПоиск обращений к PEB")
print_peb_accesses()

print("\nПоиск мест замера времени")
print_time_checks()

print("\nПоиск редких инструкций")
print_rare_instructions()

print("\nПоиск дополнительных признаков защиты")
print_protection_indicators()

print("Поиск окончен")
