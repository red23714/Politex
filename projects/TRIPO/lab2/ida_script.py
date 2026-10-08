import idaapi
import idautils
import idc
import ida_funcs
import ida_gdl
import ida_ua
import ida_bytes
import ida_idp

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

def find_xor_strings():
    strings_by_ea = {}
    for s in idautils.Strings():
        data = ida_bytes.get_strlit_contents(s.ea, s.length, s.strtype)
        if not data:
            continue
        strings_by_ea[s.ea] = (s, data)

    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)
        if not f or is_library(f):
            continue

        for ea in idautils.Heads(f.start_ea, f.end_ea):
            mnem = idc.print_insn_mnem(ea)
            if mnem not in ("lea", "mov"):
                continue

            insn = ida_ua.insn_t()
            if ida_ua.decode_insn(insn, ea) == 0:
                continue

            if len(insn.ops) < 2:
                continue
            
            _, src = insn.ops[0], insn.ops[1]

            if src.type not in (ida_ua.o_mem, ida_ua.o_imm):
                continue

            target_ea = src.addr if src.type == ida_ua.o_mem else src.value

            hit = None
            for s_ea, (s_obj, data) in strings_by_ea.items():
                if s_ea <= target_ea < s_ea + s_obj.length:
                    hit = (s_ea, s_obj, data)
                    break

            if not hit:
                continue

            s_ea, s_obj, data = hit

            print("[str 0x%X] %-10s 0x%X  %-30s" % (s_ea, mnem, ea, idc.GetDisasm(ea)))


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

print("Поиск XOR")

funcs_xor = find_xor_crypto()
for fea in funcs_xor.keys():
    print("[+] %s | 0x%X" % (idc.get_func_name(fea), fea))

    for ea in funcs_xor[fea]:
        print("      0x%X  %s" % (ea, idc.GetDisasm(ea)))

print("\nПоиск XOR-строк")

find_xor_strings()
find_byte_strings()

print("Поиск окончен")
