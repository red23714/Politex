import idc
import idautils
import idaapi
import ida_gdl
import ida_ua

def is_clearing_xor(ea):
    op1 = idc.print_operand(ea, 0)
    op2 = idc.print_operand(ea, 1)
    
    insn = ida_ua.insn_t()
    if ida_ua.decode_insn(insn, ea) > 0:
        if op1 == op2 and insn.ops[0].type == ida_ua.o_reg:
            return True
    return False

def find_xor_primitives():
    print("[*] Анализ пользовательских функций на наличие XOR-алгоритмов...")
    detected_functions = 0

    for func_ea in idautils.Functions():
        func = idaapi.get_func(func_ea)
        if not func:
            continue
            
        if func.flags & idaapi.FUNC_LIB:
            continue

        func_name = idc.get_func_name(func_ea)
        real_xor_instructions = []
        has_loop = False
        
        flowchart = ida_gdl.FlowChart(func)
        
        for block in flowchart:
            for succ_block in block.succs():
                if succ_block.start_ea <= block.start_ea and succ_block.start_ea >= func_ea:
                    current_ea = block.start_ea
        
                    while current_ea < block.end_ea:
                        disasm = idc.generate_disasm_line(current_ea, 0)
            
                        current_ea = ida_bytes.next_head(current_ea, block.end_ea)

            break

        for ea in idautils.FuncItems(func_ea):
            mnem = idc.print_insn_mnem(ea)
            
            if mnem in ["xor", "pxor"]:
                if not is_clearing_xor(ea):
                    real_xor_instructions.append(ea)

        if real_xor_instructions:
            detected_functions += 1
            loop_status = "ДА" if has_loop else "НЕТ"
            print(f"\n[+] Подозрительная функция: {func_name} ({hex(func_ea)})")
            print(f"    ├─ Наличие циклов: {loop_status}")
            print(f"    └─ Инструкции XOR ({len(real_xor_instructions)} шт.):")
            
            for xor_ea in real_xor_instructions:
                disasm = idc.generate_disasm_line(xor_ea, 0)
                print(f"        └─ {hex(xor_ea)}: {disasm}")
                
                idc.set_cmt(xor_ea, "Подозрительный XOR (возможная крипта)", 0)
            
            idc.set_cmt(func_ea, f"Возможный XOR-примитив (Циклы: {loop_status})", 1)

    print(f"\n[*] Анализ завершен.")
    print(f"    └─ Найдено подозрительных пользовательских функций: {detected_functions}")

find_xor_primitives()
