; ROTAZOOM EN CODE AUTOMODIFIE.
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

RotaZoom proc near
; Quelques explications :
; X'=x*cos(�)-y*sin(�)
; Y'=x*sin(�)+y*cos(�)
; Or on veut centrer � l'ecran, on a donc :
; X'=x*cos(�)-160*cos(�)-y*sin(�)+100*sin(�)
; Y'=x*sin(�)-160*sin(�)+y*cos(�)-100*cos(�)
; Les constantes sont calcul괻s une seule fois.
        mov     bx,angle
        shl     bx,1
        and     bh,7
        movsx   eax,w cos_table[bx]
        imul    Zoom
        shrd    eax,edx,8
        mov     edi,eax      ; edi=cosinus*2^8
        
        sub     bh,2
        and     bh,7
        movsx   eax,w cos_table[bx]
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

        add     bh,80h  ; +128   (pour centrer sur l'image en m굆oire)
        add     dh,80h  ; +128   (pour centrer sur l'image en m굆oire)
        
        mov     di,OFFSET BclAff+2 ; DI pointe sur le premier mot � modifier
        
        mov     ax,cs
        mov     es,ax
        
        mov     si,sinR
        mov     cx,cosR
        mov     al,dh
        mov     ah,bh
        mov     bp,320/2    ; parce que 4 pixels � la fois

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
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
cosR            dw ?
sinR            dw ?
zoom            dd ?
angle           dw ?
SegRotaZoom     dw ?
                                      
