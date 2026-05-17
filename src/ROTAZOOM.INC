; ROTAZOOM EN CODE AUTOMODIFIE.

RotaZoom proc near
; Quelques explications :
; X'=x*cos(α)-y*sin(α)
; Y'=x*sin(α)+y*cos(α)
; Or on veut centrer à l'ecran, on a donc :
; X'=x*cos(α)-160*cos(α)-y*sin(α)+100*sin(α)
; Y'=x*sin(α)-160*sin(α)+y*cos(α)-100*cos(α)
; Les constantes sont calculées une seule fois.
        mov     bx,angle
        shl     bx,1
        and     bh,7
        movsx   eax,word ptr cos_table[bx]
        imul    Zoom
        shrd    eax,edx,8
        mov     edi,eax      ; edi=cosinus*2^8
        
        sub     bh,2
        and     bh,7
        movsx   eax,word ptr cos_table[bx]
        imul    Zoom
        shrd    eax,edx,8
        mov     esi,eax       ; esi=sinus*2^8
        
        mov     cosR,di
        mov     sinR,si

; MODIFICATION DU CODE
        
        lea     bx,[esi+esi*4]
        shl     bx,5
        imul    dx,di,100
        add     bx,dx
        neg     bx

        movsx   ecx,cosR
        lea     cx,[ecx+ecx*4]
        shl     cx,5
        imul    dx,si,100
        sub     dx,cx

        add     bh,80h  ; +128   (pour centrer sur l'image en mémoire)
        add     dh,80h  ; +128   (pour centrer sur l'image en mémoire)
        
        mov     di,OFFSET BclAff+2 ; DI pointe sur le premier mot à modifier
        
        mov     ax,cs
        mov     es,ax
        
        mov     si,sinR
        mov     cx,cosR
        mov     al,dh
        mov     ah,bh
        mov     bp,320/2    ; parce que 4 pixels à la fois

bclm:   stosw
        add     di,2
        add     dx,cx
        add     bx,si
        mov     al,dh
        mov     ah,bh
        stosw
        add     di,3
        add     dx,cx
        add     bx,si
        mov     al,dh
        mov     ah,bh
        dec     bp
        jnz     bclm

; Fin de la modification du code.

        mov     es,Ecran
        mov     ds,SegRotazoom
                
        xor     di,di
        mov     bx,di
        mov     dx,di
        mov     cx,di
        mov     si,cs:sinR
        
        mov     bp,200

BCLAFF: REPT    160
        mov     al,[bx+1111]
        mov     ah,[bx+1111]
        stosw
        ENDM
        
        add     cx,cs:cosR
        sub     dx,si
        mov     bh,ch
        mov     bl,dh
        dec     bp
        jnz     bclaff
        
        mov     ax,cs 
        mov     ds,ax
        mov     es,ax
        ret
RotaZoom  endp


cosR            dw ?
sinR            dw ?
zoom            dd ?
angle           dw ?
SegRotaZoom     dw ?
                                      
