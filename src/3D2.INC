; /Ä
; |      PARTIE 3D Environment Mapping de 'Lion'.   |
; \Ä
; DATAS :

TRI                     = 1            ; 1=TRi QSORT  et 0=pas de tri
distance                = 10  ;(2^9=512)

;
INIT_3D                 proc    near
                        mov     es,cs:Virtuel
                        mov     ds,cs:SegAdresse1
                        xor     di,di
                        mov     si,di
                        mov     dl,200
                @transf:mov     cx,320/4
                        rep     movsd
                        sub     si,(320-256)
                        dec     dl
                        jnz     @transf
                        mov     ax,cs
                        mov     ds,ax
                        mov     es,ax
                        xor     eax,eax
                        mov     count,eax
                        mov     Xa,ax
                        mov     Ya,ax
                        mov     Za,ax
                        ret
INIT_3D                 endp

;

MAIN_3D                 proc    near
MAINBCL:                CALL    MAIN3D
                        WAIT_VBL
                        mov     es,Ecran
                        mov     ds,Virtuel
                        mov     cx,16000
                        xor     si,si
                        mov     di,si
                        rep     movsd


                        mov     es,cs:Virtuel
                        mov     ds,cs:SegAdresse1
                        xor     di,di
                        mov     si,di
                        mov     dl,200
             @transf1:  mov     cx,320/4
                        rep     movsd
                        sub     si,(320-256)
                        dec     dl
                        jnz     @transf1
                        mov     ax,cs
                        mov     es,ax
                        mov     ds,ax
                        LION

                good:   add     Xa,12
                        add     Ya,2
                        add     Za,2
                        in      al,60h
                        cmp     al,1
                        je      retour
                        inc     count
                        cmp     count,30*60
                        jne     MainBcl
                        ret
MAIN_3D                 endp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ
;Û               DEBUT DES ROUTINES DE GESTION DE LA 3D                 Û
;ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;/Ä
;| PROCEDURE PRINCIPALE DE LA 3D                                            |
;|       - rotation des points et des vecteurs,                             |
;|       - d‚termination de la visibilit‚,                                  |
;|       - tri des Faces visibles (QSORT),                                  |
;|       - affichage des Faces visibles.                                    |
;\ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ/

MAIN3D                  proc    near
; Rotation des points et des vecteurs...ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
                        CALL    PrepMatrice

                        mov     si,OFFSET LionPts
                        mov     di,OFFSET LionPtsR
                        mov     bx,OFFSET LionMatrice
                        mov     ch,b LionNb_Pts
         BclRot1:       CALL    ROTATE  ; rotation des points.
                        add     si,3
                        dec     ch
                        jnz     bclRot1
                        ; rotation points Lion

                        mov     si,OFFSET LionVctNrmPts
                        mov     di,OFFSET LionVctNrmPtsR
                        mov     bx,OFFSET LionMatrice
                        mov     ch,b LionNb_Pts
         BclRot2:       CALL    ROTATE1  ; rotation des vecteurs normaux.
                        add     si,6
                        dec     ch
                        jnz     bclRot2
                        ; rotation VctNrmPts

; projection des points … l'‚cran...ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                        CALL    XDZ

; D‚tection des Faces visibles...ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                        mov     nb_face_tri,0
                        mov     w cs:Patch+3,OFFSET FacesVisibles
                        xor     cx,cx
        mainboucle:     push    cx
                        movzx   ebp,cx
                        lea     bp,[ebp+ebp*2]

                        movzx   esi,LionFaces[bp+1]
                        shl     si,2
                        mov     ax,LionEcran[si]
                        mov     bx,LionEcran[si+2]
                        neg     ax
                        neg     bx
                        mov     cx,ax
                        mov     di,bx
                        shr     si,1
                        lea     si,[esi+esi*2]
                        mov     dx,LionPtsR[si+4]

                        movzx   si,LionFaces[bp]
                        shl     si,2
                        add     ax,LionEcran[si]
                        add     bx,LionEcran[si+2]
                        shr     si,1
                        lea     si,[esi+esi*2]
                        add     dx,LionPtsR[si+4]

                        movzx   si,LionFaces[bp+2]
                        shl     si,2
                        add     cx,LionEcran[si]
                        add     di,LionEcran[si+2]
                        shr     si,1
                        lea     si,[esi+esi*2]
                        add     dx,LionPtsR[si+4]

                        imul    ax,di
                        imul    bx,cx

                        cmp     ax,bx
                        jl      invisible

                        pop     ax
                        push    ax
                        shl     eax,16  ; met le Nø de Lion dans les 16 bits hauts
                        mov     ax,dx
                        add     ax,profondeur
                  patch:mov     dwd es:[1111],eax
                        add     w cs:Patch+3,4
                        add     nb_face_tri,4

          invisible:    pop     cx
                        inc     cx
                        cmp     cx,LionNb_Faces
                        jnz     Mainboucle
; Fin de la d‚termination des surFaces cach‚es.

; Tri des Faces...ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
                        IF      TRI EQ 1
                        CALL    Byte_Sort
                        ENDIF

; Affichage des Faces visibles...ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                        xor     si,si
        BclAffPoly:     movzx   ebx,FacesVisibles[si+2] ; BX= Nø Lion
                        lea     bx,[ebx+ebx*2]

                        movzx   ebp,LionFaces[bx]     ; bp=Nø du premier point
                        shl     bp,2
                        mov     eax,dwd LionEcran[bp]      ; ax=X
                        ror     eax,16
                        mov     dwd fy1,eax
                        shr     bp,1
                        lea     bp,[ebp+ebp*2]
                        mov     eax,dwd LionVctNrmPtsR[bp]
                        mov     dwd fVx1,eax


                        movzx   bp,LionFaces[bx+1]   ; bp=Nø du second point
                        shl     bp,2
                        mov     eax,dwd LionEcran[bp]      ; ax=X
                        ror     eax,16
                        mov     dwd fy2,eax
                        shr     bp,1
                        lea     bp,[ebp+ebp*2]
                        mov     eax,dwd LionVctNrmPtsR[bp]
                        mov     dwd fVx2,eax

                        movzx   bp,LionFaces[bx+2]   ; bp=Nø du troisiŠme point
                        shl     bp,2
                        mov     eax,dwd LionEcran[bp]      ; ax=X
                        ror     eax,16
                        mov     dwd fy3,eax
                        shr     bp,1
                        lea     bp,[ebp+ebp*2]
                        mov     eax,dwd LionVctNrmPtsR[bp]
                        mov     dwd fVx3,eax

        Gofilling:      push    si
                        call    FILL_POLY
                        pop     si

                        add     si,4
                        cmp     dwd FacesVisibles[si],0
                        jnz     BclAffPoly
                        cmp     dwd FacesVisibles[si+4],0
                        jnz     BclAffPoly
                        ret
MAIN3D                  endp


;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Pr‚pare une matrice de rotation {4,3} sur 3 axes.

PrepMatrice             proc    near
                        xor     eax,eax
                        mov     LionX,eax
                        mov     LionY,eax
                        mov     LionZ,eax
                        mov     dl,7

                        mov     bx,Xa
                        shl     bx,1
                        and     bh,dl
                        movsx   ecx,w cos_table[bx]

                        sub     bh,2
                        and     bh,dl
                        movsx   ebp,w cos_table[bx]

                        mov     bx,Ya
                        shl     bx,1
                        and     bh,dl
                        movsx   eax,w cos_table[bx]
                        mov     cosy,eax

                        sub     bh,2
                        and     bh,dl
                        movsx   eax,w cos_table[bx]
                        mov     siny,eax

                        mov     bx,Za
                        shl     bx,1
                        and     bh,dl
                        movsx   edi,w cos_table[bx]

                        sub     bh,2
                        and     bh,dl
                        movsx   esi,w cos_table[bx]

; Fin de la pr‚paration des sinus et cosinus.

                        mov     eax,edi
                        imul    cosy
                        shrd    eax,edx,10
                        mov     LionMatrice[0],eax
                        mov     eax,esi
                        imul    cosy
                        shrd    eax,edx,10
                        neg     eax
                        mov     LionMatrice[4],eax
                        mov     eax,siny
                        mov     LionMatrice[8],eax
; premiŠre ligne finie.
                        mov     eax,esi
                        imul    ecx
                        shrd    eax,edx,10
                        mov     ebx,eax
                        mov     eax,ebp
                        imul    edi
                        shrd    eax,edx,10
                        imul    siny
                        shrd    eax,edx,10
                        add     eax,ebx
                        mov     LionMatrice[16],eax

                        mov     eax,edi
                        imul    ecx
                        shrd    eax,edx,10
                        mov     ebx,eax
                        mov     eax,ebp
                        imul    esi
                        shrd    eax,edx,10
                        imul    siny
                        shrd    eax,edx,10
                        sub     ebx,eax
                        mov     LionMatrice[20],ebx
                        mov     eax,cosy
                        imul    ebp
                        shrd    eax,edx,10
                        neg     eax
                        mov     LionMatrice[24],eax
; seconde ligne finie.
                        mov     eax,esi
                        imul    ebp
                        shrd    eax,edx,10
                        mov     ebx,eax
                        mov     eax,ecx
                        imul    edi
                        shrd    eax,edx,10
                        imul    siny
                        shrd    eax,edx,10
                        sub     ebx,eax
                        mov     LionMatrice[32],ebx
                        mov     eax,edi
                        imul    ebp
                        shrd    eax,edx,10
                        mov     ebx,eax
                        mov     eax,ecx
                        imul    esi
                        shrd    eax,edx,10
                        imul    siny
                        shrd    eax,edx,10
                        add     eax,ebx
                        mov     LionMatrice[36],eax
                        mov     eax,ecx
                        imul    cosy
                        shrd    eax,edx,10
                        mov     LionMatrice[40],eax
; troisiŠme ligne finie.
                        ret
PrepMatrice             endp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Fait rotater les coordonn‚es d'un objet … partir d'une matrice de rotation.
; bx=OFFSET Matrice
; si=OFFSET PointSource
; di=OFFSET PointDestination

ROTATE                  proc    near
                        mov     cl,3

        @rot1:          movsx   eax,b [si]
                        imul    dwd [bx]
                        shrd    eax,edx,10
                        mov     ebp,eax
                        movsx   eax,b [si+1]
                        imul    dwd [bx+4]
                        shrd    eax,edx,10
                        add     ebp,eax
                        movsx   eax,b [si+2]
                        imul    dwd [bx+8]
                        shrd    eax,edx,10
                        add     eax,ebp
                        mov     [di],ax
                        inc     di
                        inc     di
                        add     bx,4*4
                        dec     cl
                        jnz     @rot1
                        sub     bx,3*(4*4)
                        ret
ROTATE                  endp

ROTATE1                 proc    near
                        mov     cl,3

        @rot2:          movsx   eax,w [si]
                        imul    dwd [bx]
                        shrd    eax,edx,10
                        mov     ebp,eax
                        movsx   eax,w [si+2]
                        imul    dwd [bx+4]
                        shrd    eax,edx,10
                        add     ebp,eax
                        movsx   eax,w [si+4]
                        imul    dwd [bx+8]
                        shrd    eax,edx,10
                        add     eax,ebp
                        mov     [di],ax
                        inc     di
                        inc     di
                        add     bx,4*4
                        dec     cl
                        jnz     @rot2
                        sub     bx,3*(4*4)
                        ret
ROTATE1                 endp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Trie le tableau des profondeurs moyennes avec l'algorithme Byte_Sort.

Byte_Sort               proc    near
                        mov     dx,1
                @loop1: mov     si,OFFSET Tab0
                        mov     di,OFFSET Tab1

                        xor     bx,bx
                @loop2: mov     eax,dwd FacesVisibles[bx]
                        test    ax,dx
                        jz      @zero
                        mov     [di],eax
                        add     di,4
                        jmp     @s1
                @zero:  mov     [si],eax
                        add     si,4
                @s1:    add     bx,4
                        cmp     bx,Nb_Face_Tri
                        jnz     @loop2

                        push    si
                        mov     si,OFFSET Tab1
                        mov     cx,di
                        sub     cx,si
                        shr     cx,2
                        mov     di,OFFSET FacesVisibles
                        rep     movsd
                        pop     cx
                        mov     si,OFFSET Tab0
                        sub     cx,si
                        shr     cx,2
                        rep     movsd
                        shl     dx,1
                        jnc     @loop1
                        xor     eax,eax
                        mov     dwd [di],eax
                        mov     dwd [di+4],eax
                        ret
Byte_Sort               endp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Transforme les coordonn‚es 3D en coordonn‚es … l'‚cran.

XDZ                     proc    near
                        mov     si,OFFSET LionPtsR
                        mov     di,OFFSET LionEcran
                        mov     bp,profondeur
                        mov     cx,LionNb_Pts
                bclXDZ: mov     bx,[si+4] ; Z
                        add     bx,bp
                        jnz     bon1
                        inc     bx
                bon1:   mov     ax,[si]   ; X
                        cwd
                        shld    dx,ax,distance
                        shl     ax,distance
                        idiv    bx
                        add     ax,160
                        stosw
                        mov     ax,[si+2]   ; Y
                        cwd
                        shld    dx,ax,distance
                        shl     ax,distance
                        idiv    bx
                        add     ax,100
                        stosw
                        add     si,6
                        dec     cx
                        jnz     bclXDZ
                        ret
XDZ                     endp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;               ATTENTION ENVIRONMENT MAPPING !!!

FILL_POLY       proc    near
                mov     fs,SegPlasma
                mov     ebx,dwd fx1

                mov     eax,dwd fy1
                cmp     ax,fy2
                jle     s_ok_1

                xchg    eax,dwd fy2
                mov     dwd fy1,eax
                mov     eax,dwd fVx1
                xchg    eax,dwd fVx2
                mov     dwd fVx1,eax

s_ok_1:         mov     eax,dwd fy2
                cmp     ax,fy3
                jle     s_ok_3

                xchg    eax,dwd fy3
                mov     dwd fy2,eax
                mov     eax,dwd fVx3
                xchg    eax,dwd fVx2
                mov     dwd fVx3,eax

s_ok_2:
                mov     eax,dwd fy1
                cmp     ax,fy2
                jle     s_ok_3

                xchg    eax,dwd fy2
                mov     dwd fy1,eax
                mov     eax,dwd fVx1
                xchg    eax,dwd fVx2
                mov     dwd fVx1,eax

s_ok_3:         mov     ax,319
                cmp     fy1,199
                jg      end_1
                cmp     fy3,0
                jl      end_1
                cmp     fx1,ax
                jb      @inter
                cmp     fx2,ax
                jb      @inter
                cmp     fx3,ax
                jb      @inter
                jmp     end_1
; Interpolation des X bordant le triangle :

        @inter: mov     bp,fy3
                mov     dx,fx3
                mov     ax,fy1
                mov     bx,fx1
                mov     di,OFFSET buffer
                call    INTERPOLE

                mov     bp,fy2
                mov     dx,fx2
                mov     ax,fy1
                mov     bx,fx1
                mov     di,OFFSET buffer+2
                call    INTERPOLE

                mov     bp,fy3
                mov     dx,fx3
                mov     ax,fy2
                mov     bx,fx2
                call    INTERPOLE

; Interpolation des X et Y de la mappe sur le grand cot‚:

                mov     ax,fVx3
                mov     bx,fVx1
                sub     ax,bx
                shl     eax,16
                cdq
                mov     cx,fy3
                sub     cx,fy1
                inc     cx
                jnz     po1
                inc     cx
          po1:  movsx   ecx,cx
                push    ecx
                idiv    ecx
                shl     ebx,16
                mov     di,OFFSET bufferX
          po2:  shld    ebp,ebx,16
                mov     [di],bp
                add     ebx,eax
                add     di,4
                dec     cx
                jnz     po2

                mov     ax,fVy3
                mov     bx,fVy1
                sub     ax,bx
                shl     eax,16
                cdq
                pop     ecx
                idiv    ecx
                shl     ebx,16
                mov     di,OFFSET bufferY
         po3:   shld    ebp,ebx,16
                mov     [di],bp
                add     ebx,eax
                add     di,4
                dec     cx
                jnz     po3

; Calcul des incr‚ments inc_X et inc_Y :

        calcinc:mov     si,fy2
                sub     si,fy1
                shl     si,2
                mov     ax,fVx2
                sub     ax,bufferX[si]
                mov     bx,fVy2
                sub     bx,bufferY[si]
                mov     cx,fx2
                sub     cx,buffer[si]
                inc     cx
                jns     fsuite51
                neg     cx
                neg     ax
                neg     bx
    fsuite51:   inc     cx
                jnz     ff10
       ff10:    movsx   ecx,cx
                cwde
                cdq
                shl     eax,8
                idiv    ecx
                mov     inc_X,ax
                movsx   eax,bx
                cdq
                shl     eax,8
                idiv    ecx
                mov     inc_Y,ax
                mov     w cs:incy+2,ax
                mov     w cs:incy1+2,ax
                mov     ax,inc_X
                mov     w cs:incx+1,ax
                mov     w cs:incx1+1,ax


                mov     bp,fy1
                mov     ax,320
                mul     bp
                mov     w cs:offdep1+2,ax
                mov     w cs:offdep2+2,ax

                mov     es,Virtuel
                mov     ax,fx2
                cmp     ax,buffer[si]
                mov     si,0
                jl      draw_next_line1

; Partie remplissant vers la droite :

draw_next_line: or      bp,bp
                jl      fnext
                cmp     bp,199
                ja      end_1

                mov     di,buffer[si]
                mov     cx,buffer[si+2]
                mov     ah,b bufferX[si]
                mov     dh,b bufferY[si]
                xor     al,al
                xor     dl,dl

                cmp     di,319
                jg      fnext
                or      cx,cx
                jl      fnext

                or      di,di
                jge     fsuite63
       fsuit1:  add     ax,inc_X
                add     dx,inc_Y
                inc     di
                jnz     fsuit1
      fsuite63: cmp     cx,319
                jle     fsuite64
                mov     cx,319

      fsuite64: sub     cx,di
                js      end_1
                inc     cx
      offdep1:  add     di,1111
                mov     bl,ah
                mov     bh,dh

      floop:    mov     bl,fs:[bx]
                mov     es:[di],bl
         incx:  add     ax,1111
         incy:  add     dx,1111
                mov     bh,dh
                inc     di
                mov     bl,ah
                dec     cx
                jnz     floop

   fnext:       add     si,4
                add     w cs:offdep1+2,320
                inc     bp
                cmp     bp,fy3
                jng     draw_next_line

                jmp     end_1           ; Fini

; Partie remplissant vers la gauche :

draw_next_line1:or      bp,bp
                js      fnext1
                cmp     bp,199
                jg      end_1
                mov     cx,buffer[si]
                mov     di,buffer[si+2]
                mov     ah,b bufferX[si]
                mov     dh,b bufferY[si]
                xor     al,al
                xor     dl,dl

                cmp     di,319
                jg      fnext1
                or      cx,cx
                js      fnext1

                cmp     cx,319
                jl      fsuite70
      fsuite71: sub     ax,inc_X
                sub     dx,inc_Y
                dec     cx
                cmp     cx,319
                jne     fsuite71

      fsuite70: or      di,di
                jge     fsuite72
                xor     di,di
      fsuite72:
                mov     bx,cx
                sub     cx,di
                js      end_1
                inc     cx
                mov     di,bx
       offdep2: add     di,1111
                mov     bl,ah
                mov     bh,dh

      floop1:   mov     bl,fs:[bx]
                mov     es:[di],bl
       incx1:   sub     ax,1111
       incy1:   sub     dx,1111
                mov     bl,ah
                dec     di
                mov     bh,dh
                dec     cx
                jnz     floop1

     fnext1:    add     si,4
                add     w cs:offdep2+2,320
                inc     bp
                cmp     bp,fy3
                jng     draw_next_line1

end_1:          ret
FILL_POLY       endp



;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Effectue une interpolation lin‚aire entre deux points donn‚s.

INTERPOLE       proc    near
                sub     bp,ax
                inc     bp
                jnz     isuite
                inc     bp

    isuite:     movsx   ebp,bp
                mov     ax,dx
                sub     ax,bx     ; ax=delta X
                cwde
                cdq
                shl     eax,16
                idiv    ebp
                shl     ebx,16
                ror     ebx,16
                ror     eax,16
                clc

Ibcl:           adc     ebx,eax
                mov     [di],bx
                inc     di
                inc     di
                inc     di
                inc     di
                dec     bp
                jnz     Ibcl
                ret
INTERPOLE       endp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Pr‚calcule les vecteurs normaux en chaque points et les norme.

GEN_NORM                proc    near
                        movzx   ecx,LionNb_Pts
                        lea     cx,[ecx+ecx*2]
                        mov     di,OFFSET LionVctNrmPts
                        xor     ax,ax
                        rep     stosw

                        mov     ebx,eax
        main1boucle:    push    bx
                        imul    bx,3
                        movzx   si,LionFaces[bx]  ; ax=Nø 1ø Pt
                        imul    si,3
                        movsx   ecx,Lionpts[si]
                        neg     ecx
                        mov     X,ecx
                        mov     X1,ecx
                        movsx   ecx,Lionpts[si+1]
                        neg     ecx
                        mov     Y,ecx
                        mov     Y1,ecx
                        movsx   ecx,Lionpts[si+2]
                        neg     ecx
                        mov     Z,ecx
                        mov     Z1,ecx

                        movzx   si,LionFaces[bx+1]
                        imul    si,3
                        movsx   ecx,Lionpts[si]
                        add     X,ecx
                        movsx   ecx,Lionpts[si+1]
                        add     Y,ecx
                        movsx   ecx,Lionpts[si+2]
                        add     Z,ecx          ; Premier vecteur fini.

                        movzx   si,LionFaces[bx+2]
                        imul    si,3
                        movsx   ecx,Lionpts[si]
                        add     X1,ecx
                        movsx   ecx,Lionpts[si+1]
                        add     Y1,ecx
                        movsx   ecx,Lionpts[si+2]
                        add     Z1,ecx          ; Second vecteur fini.

                        ; Les vecteurs de surface sont finis.

                        mov     eax,Y
                        imul    eax,Z1
                        mov     ecx,Z
                        imul    ecx,Y1
                        sub     eax,ecx
                        ; Xn fini

                        mov     ecx,Z
                        imul    ecx,X1
                        mov     edi,X
                        imul    edi,Z1
                        sub     ecx,edi
                        ; Yn fini

                        mov     edx,X
                        imul    edx,Y1
                        mov     edi,Y
                        imul    edi,X1
                        sub     edx,edi
                        ; Zn fini

                        mov     bp,3
                  @po:  movzx   si,LionFaces[bx]
                        imul    si,6
                        add     LionVctNrmPts[si],ax
                        add     LionVctNrmPts[si+2],cx
                        add     LionVctNrmPts[si+4],dx
                        inc     bx
                        dec     bp
                        jnz     @po


                        pop     bx
                        inc     bx
                        cmp     bx,LionNb_Faces
                        jnz     Main1boucle

; Calcule la norme du vecteur normal … chaque point et norme le vecteur.
; La norme de chaque vecteur est de 255.

                        xor     si,si
            main2bcl:   push    si
                        imul    si,6
                        movsx   eax,LionVctNrmPts[si]
                        movsx   ebx,LionVctNrmPts[si+2]
                        movsx   ecx,LionVctNrmPts[si+4]
                        imul    eax,eax
                        imul    ebx,ebx
                        imul    ecx,ecx
                        add     eax,ebx
                        add     eax,ecx
                        mov     ebp,eax

                        CALL    RACINE

                        mov     edi,eax
                        ;EDI = norme du vecteur
                        or      edi,edi
                        jnz     po10
                        inc     edi
                 po10:  movsx   eax,LionVctNrmPts[si]
                        movsx   ebx,LionVctNrmPts[si+2]
                        movsx   ecx,LionVctNrmPts[si+4]
                        shl     eax,7
                        cdq
                        idiv    edi
                        mov     X,eax
                        mov     eax,ebx
                        shl     eax,7
                        cdq
                        idiv    edi
                        mov     Y,eax
                        mov     eax,ecx
                        shl     eax,7
                        cdq
                        idiv    edi
                        mov     Z,eax
                        mov     eax,X
                        mov     LionVctNrmPts[si],ax
                        mov     eax,Y
                        mov     LionVctNrmPts[si+2],ax
                        mov     eax,Z
                        mov     LionVctNrmPts[si+4],ax
                        pop     si
                        inc     si
                        cmp     si,LionNb_Pts
                        jnz     main2bcl
                        mov     ax,cs
                        mov     ds,ax
                        mov     es,ax
                        ret
GEN_NORM               endp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Calcul de la racine carr‚e.
; entr‚e:EBP=carr‚
; sortie:EAX=racine

RACINE                  proc    near
                        mov     cl,7
                        mov     ebx,0FFh
             racbcl:    mov     eax,ebp
                        xor     edx,edx
                        div     ebx
                        add     eax,ebx
                        shr     eax,1
                        mov     ebx,eax
                        dec     cl
                        jnz     racbcl
                        ret
RACINE                  endp
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; DATAS
profondeur              dw 1500
LionNb_Pts              dw 88
LionNb_Faces            dw 164
LionMatrice             dd  4 dup (?)
LionX                   dd  0
                        dd  4 dup (?)
LionY                   dd  0
                        dd  4 dup (?)
LionZ                   dd  0

INCLUDE                 LION1.3D

LionEcran               dw 2*88+10 dup (?)

LionPtsR                dw 3*88+10 dup (?)

LionVctNrmPts           dw 3*88+10 dup (?)

LionVctNrmPtsR          dw 3*88+10 dup (?)

Tab0                    dd 200*2 dup (?)

Tab1                    dd 200*2 dup (?)

Nb_face_tri             dw ?
X                       dd ?
Y                       dd ?
Z                       dd ?
X1                      dd ?
Y1                      dd ?
Z1                      dd ?
Xa                      dw ?
Ya                      dw ?
Za                      dw ?
cosy                    dd ?
siny                    dd ?
fy1                     dw ?
fx1                     dw ?
fVx1                    dw ?
fVy1                    dw ?
fy2                     dw ?
fx2                     dw ?
fVx2                    dw ?
fVy2                    dw ?
fy3                     dw ?
fx3                     dw ?
fVx3                    dw ?
fVy3                    dw ?
inc_X                   dw ?
inc_Y                   dw ?
popo                    dw ?
popo1                   dw ?
Buffer                  dw 400 dup(?)
BufferX                 dw 400 dup(?)
BufferY                 dw 400 dup(?)
FacesVisibles           dw 200*2 dup (?)

