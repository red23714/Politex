import sys
import angr
from angr import claripy

# ---------------------------------------------------------------------------
# НАСТРОЙКИ
# ---------------------------------------------------------------------------
BINARY_PATH   = "./crackme32.exe"
FUNC_ADDR     = 0x00401570
BUF_LEN       = 40
PASSWORD_LEN  = 37
FAKE_RET_ADDR = 0x7FFFFFFF
BUF_ADDR      = 0x70000000   # любой свободный адрес

# Адреса из IDA
COMPARE_LOOP_ADDR = 0x004016C0   # начало цикла сравнения (loc_4016C0)


def main():
    print(f"[*] Загружаю бинарник: {BINARY_PATH}")
    proj = angr.Project(BINARY_PATH, auto_load_libs=False)

    # ------------------------------------------------------------------
    # Хук на начало цикла сравнения.
    #
    # Оригинал делает 37 раз:
    #     movzx ecx, [ebp+eax+var_2C]   ; symbol
    #     movzx eax, [ebp+edx+var_54]   ; concrete
    #     cmp   ecx, eax
    #     jz    ...
    # и на каждом шаге форкает состояние -> до 2**37 путей.
    #
    # Мы заменяем это одной constraint: var_2C[i] == var_54[i] для всех i.
    # После этого все cmp в цикле становятся тривиально истинными,
    # angr идёт по одной ветке до "mov eax, 1".
    # ------------------------------------------------------------------
    hook_hits = [0]

    def hook_compare(state):
        hook_hits[0] += 1
        ebp_val = state.regs.ebp
        # var_2C: трансформированный ввод
        # var_54: эталон (скопирован из unk_41EE50)
        for i in range(PASSWORD_LEN):
            sym = state.memory.load(ebp_val - 0x2C + i, 1)
            tgt = state.memory.load(ebp_val - 0x54 + i, 1)
            state.solver.add(sym == tgt)

    proj.hook(COMPARE_LOOP_ADDR, hook_compare, length=0)
    print(f"[*] Хук на {hex(COMPARE_LOOP_ADDR)} -- заменяем цикл на constraint")

    # ------------------------------------------------------------------
    # Символьный буфер
    # ------------------------------------------------------------------
    password_chars = [claripy.BVS(f"pw_{i}", 8) for i in range(BUF_LEN)]

    state = proj.factory.call_state(
        FUNC_ADDR,
        BUF_ADDR,
        ret_addr=FAKE_RET_ADDR,
        prototype="int f(char*)",
        add_options={
            angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
            angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS,
        },
    )

    # Записываем байты в память по одному -- правильный порядок
    for i in range(BUF_LEN):
        state.memory.store(BUF_ADDR + i, password_chars[i])

    # Ограничения: [0-9A-Za-z]
    for i in range(PASSWORD_LEN):
        c = password_chars[i]
        state.solver.add(
            claripy.Or(
                claripy.And(c >= ord('0'), c <= ord('9')),
                claripy.And(c >= ord('a'), c <= ord('z')),
                claripy.And(c >= ord('A'), c <= ord('Z')),
            )
        )
    state.solver.add(password_chars[PASSWORD_LEN] == 0)

    # ------------------------------------------------------------------
    # Запуск
    # ------------------------------------------------------------------
    print(f"[*] Символьное исполнение sub_401570 @ {hex(FUNC_ADDR)}...")
    simgr = proj.factory.simulation_manager(state)
    simgr.explore(find=FAKE_RET_ADDR, num_find=1)

    print(f"[*] Хук сработал раз: {hook_hits[0]}")

    if not simgr.found:
        print("[-] Не дошли до ret. Проверьте адрес хука / базу загрузки.")
        sys.exit(1)

    solution = simgr.found[0]

    # Проверяем, что eax действительно может быть 1
    if not solution.solver.satisfiable(extra_constraints=[solution.regs.eax == 1]):
        print("[-] Состояние на ret имеет eax != 1.")
        print(f"    eax = {solution.solver.eval(solution.regs.eax)}")
        sys.exit(1)

    solution.solver.add(solution.regs.eax == 1)
    print("[+] Состояние на ret достигнуто, eax == 1")

    # ------------------------------------------------------------------
    # Извлекаем байты
    # ------------------------------------------------------------------
    solved_bytes = b''.join(
        solution.solver.eval(solution.memory.load(BUF_ADDR + i, 1), cast_to=bytes)
        for i in range(BUF_LEN)
    )

    nul  = solved_bytes.find(b"\x00")
    core = solved_bytes[:nul] if nul != -1 else solved_bytes

    print()
    print("[+] Пароль найден:")
    print("    hex  :", core.hex())
    try:
        print("    ascii:", core.decode("ascii"))
    except UnicodeDecodeError:
        print("    (не ASCII)")

    try:
        full_input = core.decode("ascii") + "k"
        print()
        print("    Полная строка для ввода (main требует хвостовой 'k'):")
        print("       ", full_input)
    except UnicodeDecodeError:
        pass


if __name__ == "__main__":
    main()
