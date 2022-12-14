# ch55x_asm_doc
## CH55X assembly instruction description

Applicable models include: CH551, CH552, CH554, CH558, CH559

Overview of the CH55X assembly instruction cycle:

1. The number of instruction cycles for a non-jump instruction is the same as the number of instruction bytes (except for the DIV instruction of CH551/2/3/4).

2. Most instructions are single-byte, 1T single-cycle instructions.

3. Jump instructions with MOVC/RET/CALL are usually several more cycles than the number of instruction bytes.

4. RET/RETI instruction starts from 3 cycles, for example, the return address is odd and 1 cycle.

5. The `MOVC` instruction starts from 4 cycles. If the address of this instruction is odd and adds 1 cycle, the following instruction address is odd and adds 1 cycle.

6. `JMP @A` instruction starts from 2 cycles, if the address of this instruction is odd and adds 1 cycle, if the target address is odd and add 1 cycle.

7. JB/JNB/JBC/CJNE_A, dir/DJNZ instruction starts from 2 cycles, if the address of this instruction is odd and add 1 cycle, if the target address is odd and add 1 cycle.

8. The remaining jump instructions start from 2 cycles, such as the target address is odd and 1 cycle.

9. The conditional jump instruction has the same number of cycles as the instruction byte if no jump occurs.

**Note: The above period refers to the reciprocal of the current system frequency of the current CH55X.**


### Transfer instructions (29)

| Instruction format | Functional description | Instruction byte | Instruction cycle |
|--------------------|------------------------|------------------|-------------------|
| MOV A,Rn | The contents of (Rn)→(A)Rn are move to accumulator A, Rn=R0-R7 | 1 | 1 |
| MOV A,direct | (direct) → (A) the contents of the direct unit address are move to the accumulator A | 2 | 2 |
| MOV A,@Ri | (Ri) → (A) The contents of the address unit pointed to by the Ri content are move to the accumulator A, Ri=R0 or R1 | 1 | 1 |
| MOV A,#data | #data→(A) Immediately move to accumulator A | 2 | 2 |
| MOV Rn,A | (A)→(Rn) The contents of accumulator A are move to register Rn | 1 | 1 |
| MOV Rn,direct | (direct)→(Rn) directly address the contents of the unit to send registers | 2 | 2 |
| MOV Rn,#data | #data→(Rn) immediate data is move directly to register Rn | 2 | 2 |
| MOV direct,A | (A)→(direct) accumulator A content is move to the direct addressing unit | 2 | 2 |
| MOV direct,Rn | The contents of the (Rn)→(direct) register are move to the directly addressed unit. | 2 | 2 |
| MOV direct2,direct1 | (direct1)→(direct2) Directly address the contents of the unit 1 Send the direct addressing unit 2 | 3 | 3 |
| MOV direct,@Ri | The contents of the (Ri)→(direct) register are move to the direct addressing unit. | 2 | 2 |
| MOV direct,#data | #data→(direct) immediate number to send direct addressing unit | 3 | 3 |
| MOV @Ri,A | (A)→((Ri)) The contents of accumulator A are move to the RAM unit with the address in Ri as the address | 1 | 1 |
| MOV @Ri,direct | (direct)→((Ri)) Directly address the contents of the unit to the RAM unit addressed to the contents of Ri | 2 | 2 |
| MOV @Ri,#data | #data→((Ri)) immediate data is move to the RAM unit with the content in Ri as the address | 2 | 2 |
| MOV DPTR,#data16 | #dataH→(DPH), #dataL→(DPL) The upper 8 bits of the 16-bit constant are move to DPH, and the lower 8 bits are move to DPL. | 3 | 3 |
| MOVX A,@DPTR | ((DPTR)) → (A) The data pointer points to the contents of the external RAM (16-bit address) to the accumulator A | 1 | 1 |
| MOVX A,@Ri | ((Ri))→(A) Register Ri The contents of the external RAM (8-bit address) are move to accumulator A, and the upper 8 bits are provided by P2. | 1 | 1 |
| MOVX @DPTR,A | (A)→((DPTR)) The contents of the accumulator are move to the data pointer to the external RAM (16-bit address) | 1 | 1 |
| MOVX @Ri,A | (A)→((Ri)) The contents of the accumulator are move to the register Ri to the external RAM (8-bit address), and the upper 8 bits are provided by P2 | 1 | 1 |
| MOVC A,@A+DPTR | ((A)+(DPTR))→(A) The contents of the table address unit are move to the accumulator A. | 1 |(*) 5/6/7 |
| MOVC A,@A+PC | (PC)+1→(PC),((A)+(PC))→(A) The contents of the table address unit are move to accumulator A | 1 |(*) 5/6/7 |
| XCH A,Rn | (A)←→(Rn) accumulator and content exchange in working register Rn | 1 | 1 |
| XCH A,direct | (direct)←→(A) The contents of the accumulator A and the content exchange of the directly addressed unit | 2 | 2 |
| XCH A,@Ri | (A)←→((Ri)) The contents of the accumulator are interchanged with the contents of the working register Ri | 1 | 1 |
| XCHD A,@Ri | (A[3:0])←→((Ri)[3:0]) The accumulator is swapped with the lower bit of the contents of the memory location pointed to by the working register Ri | 1 | 1 |
| SWAP A | (A[3:0])←→(A[7:4]) The contents of the accumulator are swapped in high and low bits | 1 | 1 |
| PUSH direct | (SP)+1→(SP),(direct)→(SP) The stack pointer is first incremented by 1, and the data in the direct addressing unit is move to the unit pointed to by the stack pointer SP. | 2 | 2 |
| POP direct | (SP)→(direct), (SP)-1→(SP) The stack pointer refers to the unit data move to the direct addressing unit, and the stack pointer SP is decremented by 1 | 2 | 2 |


### Arithmetic operation instructions (24)

| Instruction format | Functional description | Instruction byte | Instruction cycle |
|--------------------|------------------------|------------------|-------------------|
| ADD A,Rn | (A)+(Rn)→(A) The contents of accumulator A are added to the contents of working register Rn, and the result exists in A. | 1 | 1 |
| ADD A,direct | (A)+(direct)→(A) The contents of accumulator A are added to the contents of the direct address element, and the result exists in A. | 2 | 2 |
| ADD A,@Ri | (A)+((Ri))→(A) The contents of accumulator A are added to the contents of the address unit pointed to by working register Ri, and the result exists in A. | 1 | 1 |
| ADD A,#data | (A)+#data→(A) The contents of accumulator A are added to the immediate #data, and the result exists in A | 2 | 2 |
| ADDC A,Rn | (A)+Rn+(C)→(A) The contents of accumulator A are added to the contents of working register Rn, together with the carry bit, and the result exists in A. | 1 | 1 |
| ADDC A,direct | (A)+(direct)+(C)→(A) The contents of the accumulator A are added to the contents of the direct address unit together with the carry bit, and the result exists in A | 2 | 2 |
| ADDC A,@Ri | (A)+((Ri))+(C)→(A) The contents of accumulator A and the working register Ri point to the contents of the address unit, together with the carry bit, and the result exists in A | 1 | 1 |
| ADDC A,#data | (A)+#data+(C)→(A) The contents of accumulator A are added to the immediate value along with the carry bit, and the result exists in A. | 2 | 2 |
| INC A | (A)+1→(A) The content in accumulator A is incremented by 1, and the result exists in A | 1 | 1 |
| INC Rn | (Rn) +1 → (Rn) register The content of Rn is incremented by 1, and the result is move back to the original address unit. | 1 | 1 |
| INC direct | (direct)+1→(direct) The content in the direct address unit is incremented by 1, and the result is move back to the original address unit. | 2 | 2 |
| INC @Ri | ((Ri)) +1→((Ri)) The contents of the address unit pointed to by the contents of the register are incremented by 1, and the result is move back to the original address unit. | 1 | 1 |
| INC DPTR | (DPTR) +1 → (DPTR) data pointer content plus 1, the result is move back to the data pointer | 1 | 1 |
| SUBB A,Rn | (A)-(Rn)-(C)→(A) The contents of accumulator A are subtracted from the contents of the working register, along with the borrowing bits. The result is in A. | 1 | 1 |
| SUBB A,direct | (A)-(direct)-(C)→(A) The contents of the accumulator A are subtracted from the contents of the direct address unit, together with the borrowing bit, and the result exists in A | 2 | 2 |
| SUBB A,@Ri | (A)-((Ri))-(C)→(A) The contents of the accumulator A are subtracted from the contents of the address unit pointed to by the working register Ri, together with the borrowing bit, and the result exists in A. | 1 | 1 |
| SUBB A,#data | (A)-#data-(C)→(A) The contents of accumulator A are subtracted from the immediate value, along with the borrowing bit, and the result exists in A. | 2 | 2 |
| DEC A | (A)-1→(A) The contents of the accumulator A are decremented by 1, and the result is move back to the accumulator A. | 1 | 1 |
| DEC Rn | (Rn)-1→(Rn) Register The contents of Rn are decremented by 1, and the result is move back to the register Rn. | 1 | 1 |
| DEC direct | (direct)-1→(direct) The content in the direct address unit is decremented by 1, and the result is move back to the direct address unit. | 2 | 2 |
| DEC @Ri | ((Ri))-1→((Ri)) register The content of the address unit pointed to by Ri is decremented by 1, and the result is move back to the original address unit. | 1 | 1 |
| MUL AB | (A) × (B) → (A) and (B) The contents of accumulator A are multiplied by the contents of register B, and the result exists in A, B. | 1 | 1 |
| DIV AB (CH558/559) | (A) ÷ (B) → (A) and (B) The contents of accumulator A are divided by the contents of register B, the resulting quotient is accumulator A, and the remainder is stored in register B. | 1 | 1 |
| DIV AB (CH551/552/554) | (A) ÷ (B) → (A) and (B) The contents of accumulator A are divided by the contents of register B, the resulting quotient is accumulator A, and the remainder is stored in register B. | 1 |(*) 4 |
| DA A | Accumulator for decimal conversion | 1 | 1 |



### Logical operation instructions (24)

| Instruction format | Functional description | Instruction byte | Instruction cycle |
|--------------------|------------------------|------------------|-------------------|
| ANL A,Rn | The contents of accumulator A and the contents of register Rn are executed and logically operated. The result is in accumulator A. | 1 | 1 |
| ANL A,direct | The contents of accumulator A and the contents of the direct address unit are executed and logically operated, and the result is stored in register A. | 2 | 2 |
| ANL A,@Ri | The contents of accumulator A and the contents of the address unit pointed to by the working register Ri are executed and logically operated. The result is stored in accumulator A. | 1 | 1 |
| ANL A,#data | The contents of the accumulator A and the immediate execution and logical operations. The result is in accumulator A | 2 | 2 |
| ANL direct,A | The contents of the direct address unit and the contents of accumulator A are executed and logically operated, and the result is in the direct address unit. | 2 | 2 |
| ANL direct,#data | The contents of the direct address unit and the immediate execution and logical operations, the result is in the direct address unit | 3 | 3 |
| ORL A,Rn | The contents of accumulator A and the contents of register Rn perform a logical OR operation, and the result is in accumulator A. | 1 | 1 |
| ORL A,direct | The contents of accumulator A and the contents of the direct address unit perform a logical OR operation, and the result is stored in register A. | 2 | 2 |
| ORL A,@Ri | The contents of accumulator A and the contents of the address unit pointed to by the working register Ri perform a logical OR operation, and the result is in accumulator A. | 1 | 1 |
| ORL A,#data | The contents of accumulator A and the immediate value perform a logical OR operation, and the result exists in accumulator A | 2 | 2 |
| ORL direct,A | The contents of the direct address unit and the contents of accumulator A perform a logical OR operation, and the result exists in the direct address unit. | 2 | 2 |
| ORL direct,#data | The contents of the direct address unit and the immediate data perform a logical OR operation, and the result exists in the direct address unit. | 3 | 3 |
| XRL A,Rn | The contents of accumulator A and the contents of register Rn perform a logical XOR operation, and the result is in accumulator A. | 1 | 1 |
| XRL A,direct | The contents of accumulator A and the contents of the direct address unit perform a logical XOR operation, and the result is stored in register A. | 2 | 2 |
| XRL A,@Ri | The contents of accumulator A and the contents of the address unit pointed to by the working register Ri perform a logical XOR operation, and the result is in accumulator A. | 1 | 1 |
| XRL A,#data | The contents of accumulator A and the immediate data are logically XORed, and the result is in accumulator A. | 2 | 2 |
| XRL direct,A | The contents of the direct address unit and the contents of accumulator A perform a logical XOR operation, and the result exists in the direct address unit. | 2 | 2 |
| XRL direct,#data | The content in the direct address unit and the immediate data perform a logical XOR operation, and the result exists in the direct address unit. | 3 | 3 |
| CLR A | 0→(A), the contents of the accumulator are cleared. 0 | 1 | 1 |
| CPL A | The contents of the accumulator are reversed by bit | 1 | 1 |
| RL A | The content in accumulator A is shifted one bit to the left | 1 | 1 |
| RR A | The contents of accumulator A are shifted to the right by one | 1 | 1 |
| RLC A | The contents of accumulator A are shifted left by one bit together with the carry bit CY | 1 | 1 |
| RRC A | The contents of accumulator A are shifted to the right by one bit together with the carry bit CY | 1 | 1 |


### Control transfer instruction (16)

| Instruction format | Functional description | Instruction byte | Instruction cycle |
|--------------------|------------------------|------------------|-------------------|
| LJMP addr16 | Addr16→(PC), giving the program counter a new value (16-bit address) | 3 |(*) 5/6 |
| AJMP addr11 | (PC)+2→(PC), addr11→(PC[10:0]) The program counter gives a new value (11-bit address), and (PC[15:11]) does not change. | 2 |(*) 4/5 |
| SJMP rel | (PC)+2+rel→(PC) The current program counter is first added 2 plus the offset gives the program counter a new value. | 2 |(*) 4/5 |
| JMP @A+DPTR | (A) + (DPTR) → (PC), the value of the address unit pointed to by the accumulator plus the value of the data pointer gives the program counter a new value. | 1 |(*) 3/4/5 |
| JZ rel | A=0, (PC)+2+rel→(PC), if the content in the accumulator is 0, it will transfer to the address pointed to by the offset, otherwise the program will execute downward. | 2 |(*) 2 or 4/5 |
| JNZ rel | A≠0, (PC)+2+rel→(PC), if the content in the accumulator is not 0, it will be transferred to the address pointed to by the offset, otherwise the program will execute downward. | 2 |(*) 2 or 4/5 |
| CJNE A,#data,rel | A≠#data,(PC)+3+rel→(PC), if the content in the accumulator is not equal to the immediate value, it will be transferred to the address pointed to by the offset, otherwise the program will execute downward. | 3 |(*) 3 or 5/6 |
| CJNE Rn,#data,rel | A≠#data,(PC)+3+rel→(PC), the content in the working register Rn is not equal to the immediate value, then it is transferred to the address pointed to by the offset, otherwise the program executes downward. | 3 |(*) 3 or 5/6 |
| CJNE @Ri,#data,rel | A≠#data,(PC)+3+rel→(PC), the working register Ri points to the address unit and the content is not equal to the immediate value, then it is transferred to the address pointed to by the offset, otherwise the program goes down. | 3 |(*) 3 or 5/6 |
| CJNE A,direct,rel | A≠(direct), (PC)+3+rel→(PC), the content in the accumulator is not equal to the content of the direct address unit, then it is transferred to the address pointed to by the offset, otherwise the program executes downward. | 3 |(*) 3 or 5/6/7 |
| DJNZ Rn,rel | (Rn)-1→→(Rn),(Rn)≠0,(PC)+2+rel→(PC) working register Rn minus 1 is not equal to 0, then it is transferred to the address pointed to by the offset, otherwise the program goes to Execution | 2 |(*) 2 or 4/5/6 |
| DJNZ direct,rel | (direct)-1 → (direct), (direct) ≠ 0, (PC) + 2 + rel → (PC) direct address unit content minus 1 is not equal to 0, then transfer to the address pointed to by the offset, Otherwise the program goes down | 3 |(*) 3 or 5/6/7 |
| LCALL addr16 | The long call instruction can call the subroutine in 64KB space. At this time (PC)+3→(PC), (SP)+1→(SP), (PC[7:0])→(SP), (SP)+1→(SP), (PC[15: 8])→(SP), addr16→(PC), which is the return address pushed in when the subroutine is called from the stack. | 3 |(*) 5/6 |
| ACALL addr11 | Absolute call instruction, subroutine can be called in 2KB space, at this time (PC)+2→(PC), (SP)+1→(SP),(PC[7:0])→(SP),(SP) +1→(SP),(PC[15:8])→(SP), addr11→(PC[10:0]) | 2 |(*) 4/5 |
| RET | Subroutine return instruction | 1 |(*) 4/5 |
| RETI | Interrupt return instruction, RETI instruction cannot be replaced by RET | 1 |(*) 4/5 |



### Bit operation instruction (18)

| Instruction format | Functional description | Instruction byte | Instruction cycle |
|--------------------|------------------------|------------------|-------------------|
| MOV C,bit | Bit→CY, a bit of data is move to CY | 2 | 2 |
| MOV bit,C | CY→bit, CY data to send a bit | 2 | 2 |
| CLR C | 0→CY, clear CY | 1 | 1 |
| CLR bit | 0→bit, clear one bit | 2 | 2 |
| SETB C | 1→CY, set CY | 1 | 1 |
| SETB bit | 1→bit, set a bit | 2 | 2 |
| CPL C | CY inversion | 1 | 1 |
| CPL bit | Bit inversion | 2 | 2 |
| ANL C,bit | CY and bit are matched, and the result is stored in CY | 2 | 2 |
| ANL C,/bit | The inverse of CY and bit is the same, and the result is stored in CY. | 2 | 2 |
| ORL C,bit | CY and bit are OR, and the result is stored in CY | 2 | 2 |
| ORL C,/bit | The inverse of CY and bit is OR, and the result is stored in CY | 2 | 2 |
| JC rel | CY)=1 transfer, (PC)+2+rel→PC, otherwise the program goes down, (PC)+2→PC | 2 |(*) 2 or 4/5 |
| JNC rel | (CY)=0 transfer, (PC)+2+rel→PC, otherwise the program will execute downward, (PC)+2→PC | 2 |(*) 2 or 4/5 |
| JB bit,rel | The bit status is 1 transfer, (PC)+3+rel→PC, otherwise the program goes down, (PC)+3→PC | 3 |(*) 3 or 5/6/7 |
| JNB bit,rel | The bit status is 0, (PC)+3+rel→PC, otherwise the program goes down, (PC)+3→PC | 3 |(*) 3 or 5/6/7 |
| JBC bit,rel | The bit status is 1 transition, and the bit is cleared to 0, (PC) + 3 + rel → PC, otherwise the program goes down, (PC) + 3 → PC | 3 |(*) 3 or 5/6/7 |
| NOP | Empty instruction, this instruction does not perform any operation except for one cycle of PC plus one cycle, which can be used for short time delay. | 1 | 1 |


### New instruction (1)

| Instruction format | Functional description | Instruction byte | Instruction cycle |
|--------------------|------------------------|------------------|-------------------|
| DB 0xA5 | XRAM fast copy instruction | 1 | 1 |


**Note: The mark is (*), which means that the number of instruction bytes is not equal to the cycle required to execute the instruction.**

Example of new instruction usage:
```
;New Instruction: MOVX @DPTR1,A
;Instruction Code: 0xA5
;Instruction Cycle: 1
;Instruction Operation:
    ;step-1. write ACC into xdata SRAM @DPTR1 embedded chip
    ;step-2. increase DPTR1
ASM example:
    INC XBUS_AUX
    MOV DPTR,#TARGET_ADDR ;DPTR1
    DEC XBUS_AUX
    MOV DPTR,#SOURCE_ADDR ;DPTR0
    MOV R7,#xxH
LOOP: MOVX A,@DPTR        ;DPTR0
    INC DPTR              ;DPTR0, if need
    DB 0A5H               ;MOVX @DPTR1,A & INC DPTR1
    DJNZ R7,LOOP
```