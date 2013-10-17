; -------------------------------------------------
; 	线程切换的实现
;
; 	hurley 2013/10/10
;
; -------------------------------------------------

[global switch_thread]
[extern current_thread]
[extern thread_exit]

; 具体的线程切换操作，重点在于寄存器的保存与恢复
switch_thread:
        mov eax, [current_thread]
        mov eax, [eax]

        mov [eax+0],  esp
        mov [eax+4],  ebp
        mov [eax+8],  ebx
        mov [eax+12], esi
        mov [eax+16], edi
        
        pushf
        pop ecx
        mov [eax+20], ecx

        mov eax, [esp+4]

        mov [current_thread], eax
        mov eax, [eax]
        
        mov esp, [eax+0]
        mov ebp, [eax+4]
        mov ebx, [eax+8]
        mov esi, [eax+12]
        mov edi, [eax+16]
        
        mov eax, [eax+20]
        push eax
        popf
        
        ret

