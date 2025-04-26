; Code du tunnel mapp‚.

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Proc‚dure principale du tunnel.
TUNNEL                  proc    near
                        mov     es,Ecran
                        mov     fs,SegAdresse1
                        mov     dx,Decal
                        push    dx
                        shl     edx,16
                        pop     dx
                        mov     ds,SegPlasma
                        mov     cx,16000
                        xor     di,di
                        mov     si,di
                        CALL    TUN
                        xor     si,si
                        mov     fs,cs:SegAdresse2
                        mov     cx,16000
                        CALL    TUN
                        mov     ax,cs
                        mov     ds,ax
                        mov     es,ax
                        ret
TUNNEL                  endp
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Procedure d'affichage du tunnel.
TUN                     proc    near
                @aff:   mov     ebx,fs:[si]     ; BX=adresse
                        add     ebx,edx
                        mov     al,[bx]     ; AL=couleur
                        shr     ebx,16
                        mov     ah,[bx]     ; AL=couleur
                        mov     es:[di],ax
                        inc     di
                        inc     di
                        add     si,4
                        dec     cx
                        jnz     @aff
                        ret
TUN                     endp
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Pr‚pare les adresses dans les segments SegAdresse1 et SegAdresse2, 
; en enroulant chaque ligne de la mappe 256*256 sur un cercle dont le rayon 
; croit.

GEN_ADRESSE             proc    near
                        mov     fs,SegAdresse1
                        mov     gs,SegAdresse2
                        
                @2:     xor     bx,bx
                        mov     ax,2500
                        cwd
                        mov     cx,Rayon
                        shr     cx,3
                        add     cx,5
; Changement de l'effet de perspective en changeant 5
                        idiv    cx
                        mov     dh,al
                        mov     dl,bl
                        mov     w adresse+2,dx
                        
                @1:     mov     ax,w Cos_Table[bx]
                        imul    Rayon
                        shrd    ax,dx,10+1+1+1
                        cwde
                        add     eax,160
                        mov     edi,eax
                        cmp     ax,319
                        ja      @next
                        
                        lea     si,[bx-512]
                        and     si,2047
                        mov     ax,w Cos_Table[si]
                        imul    Rayon
                        shrd    ax,dx,10+1+1+1
                        cwde
                        add     eax,100
                        cmp     ax,199
                        ja      @next
                        
                        lea     eax,[eax+eax*4]
                        shl     eax,6
                        add     edi,eax
                        shl     edi,1
                        mov     ax,w adresse+2
                        cmp     edi,64000
                        jb      @fs
                        add     di,256+320*2*2
                        mov     gs:[di],ax
                        jmp     @next
                  @fs:  mov     fs:[di],ax
               @next:   add     adresse,08000h
                        inc     bx
                        inc     bx
                        cmp     bx,2048
                        jne     @1
                        inc     Rayon
                        cmp     Rayon,190*2*2*2
                        jnz     @2
                        mov     ax,cs
                        mov     ds,ax
                        mov     es,ax
                        ret
GEN_ADRESSE             endp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;  /ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\
;  ³°°°°°°°°°°°°°°°°°°° Datas °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°³
;  \ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ/

Rayon                   dw 0
SegAdresse1             dw ?
SegAdresse2             dw ?
Decal                   dw ?
adresse                 dd ?
