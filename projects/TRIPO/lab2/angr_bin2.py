import sys
import angr
from angr import claripy

# --- фикс bitstring 4.x ---
import bitstring
if not hasattr(bitstring, "ConstBitStream"):
    bitstring.ConstBitStream = bitstring.Bits
# --------------------------

BINARY_PATH = "./crackme32_1.exe"

SUB_401190  = 0x00401190
SUB_401000  = 0x00401000
BUF_ADDR    = 0x0041B020
BYTE_TABLE  = 0x0041B000
AJU129      = 0x0041B010
A0IDSWQ     = 0x0041B018
PASSWORD_LEN = 14
FAKE_RET    = 0x7FFFFFFF

SEH_TRAMPOLINES = {
    0x0040120B: 0x0040122A,
    0x00401292: 0x004012FA,
    0x004012A5: 0x004012C1,
}


def fill_from_table(state, dst, parity):
    """Заполняет dst 7 байтами byte_41B000 с чётными (parity=0)
    или нечётными (parity=1) индексами + нулевой терминатор."""
    for i in range(7):
        b = state.memory.load(BYTE_TABLE + 2 * i + parity, 1)
        state.memory.store(dst + i, b)
    state.memory.store(dst + 7, claripy.BVV(0, 8))


class Sub401000(angr.SimProcedure):
    """Замена sub_401000: накладывает constraint и возвращает 1."""
    def run(self, arg0, arg4):
        for i in range(7):
            b0 = self.state.memory.load(arg0 + i, 1)
            b4 = self.state.memory.load(arg4 + i, 1)
            inp_lo = self.state.memory.load(BUF_ADDR + i, 1)
            inp_hi = self.state.memory.load(BUF_ADDR + 7 + i, 1)
            self.state.solver.add(inp_lo == b0)
            self.state.solver.add(inp_hi == b4)
        return 1


def main():
    proj = angr.Project(BINARY_PATH, auto_load_libs=False)
    password_chars = [claripy.BVS(f"pw_{i}", 8) for i in range(PASSWORD_LEN)]

    # --- SEH-хуки ---
    for addr, target in SEH_TRAMPOLINES.items():
        def make_hook(tgt):
            def hook(state):
                eip = state.solver.eval(state.regs.eip)
                print(f"  [SEH] {hex(eip)} → {hex(tgt)}")
                if eip == 0x0040120B:
                    # В реальности SEH-фильтр loc_401221 вызывает sub_401130,
                    # которая заполняет aJU129 чётными индексами byte_41B000.
                    fill_from_table(state, AJU129, parity=0)
                    print("  [SEH] aJU129 ← byte_41B000[чётные] = 'kb#fwj1'")
                state.regs.eip = tgt
            return hook
        proj.hook(addr, make_hook(target), length=10)
        print(f"[*] SEH-хук: {hex(addr)} → {hex(target)}")

    # --- Хук после loc_40124C: принудительно выставляем a0iDswq ---
    @proj.hook(0x00401266, length=0)
    def fix_a0idswq(state):
        print("  [FIX] a0iDswq ← byte_41B000[нечётные] = 'P@bbfb2'")
        fill_from_table(state, A0IDSWQ, parity=1)

    # --- sub_401000 → SimProcedure ---
    proj.hook(SUB_401000, Sub401000())
    print(f"[*] sub_401000 → SimProcedure")

    # --- Старт с sub_401190 ---
    state = proj.factory.call_state(
        SUB_401190,
        ret_addr=FAKE_RET,
        prototype="int f()",
        add_options={
            angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
            angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS,
        },
    )
    for i in range(PASSWORD_LEN):
        state.memory.store(BUF_ADDR + i, password_chars[i])
    state.memory.store(BUF_ADDR + PASSWORD_LEN, claripy.BVV(0, 8))

    for i in range(PASSWORD_LEN):
        c = password_chars[i]
        state.solver.add(claripy.And(c >= 0x20, c <= 0x7E))

    simgr = proj.factory.simulation_manager(state)
    print("[*] Ищу путь до ret...")
    simgr.explore(find=FAKE_RET, num_find=1)

    if not simgr.found:
        print("[-] Не найдено")
        print(f"    deadended: {len(simgr.deadended)}, "
              f"errored: {len(simgr.errored)}, "
              f"active: {len(simgr.active)}")
        for s in simgr.errored[:5]:
            print("    error:", s.error)
        for s in simgr.deadended[:5]:
            print("    dead eip:", hex(s.solver.eval(s.regs.eip)),
                  "eax:", s.solver.eval(s.regs.eax))
        return

    sol = simgr.found[0]
    print("[+] eax =", sol.solver.eval(sol.regs.eax))

    pw = b"".join(
        sol.solver.eval(password_chars[i], cast_to=bytes)
        for i in range(PASSWORD_LEN)
    )
    print()
    print("[+] Пароль найден:")
    print("    hex  :", pw.hex())
    print("    ascii:", pw.decode("latin-1"))


if __name__ == "__main__":
    main()
