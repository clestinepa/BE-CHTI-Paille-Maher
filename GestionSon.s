	PRESERVE8
	THUMB   
	
	EXPORT CallbackSon
	EXPORT StartSon
	EXPORT SortieSon
	EXPORT Index
	import Son
	import LongueurSon
	include Driver/DriverJeuLaser.inc ;pour PWM


; ====================== zone de r�servation de donn�es,  ======================================
;Section RAM (read only) :
	area    mesdata,data,readonly


;Section RAM (read write):
	area    maram,data,readwrite
		
SortieSon dcw 0 ; short integer sur 2 octets

;avant StartSon :
;Index dcd 0 ;integer sur 4 octets
;apr�s StartSon :
Index dcd 5511 ;on commence � la fin du son, pour ne pas le jouer � chaque lancement de programme 

; ===============================================================================================
	


		
;Section ROM code (read only) :		
	area    moncode,code,readonly
; �crire le code ici		

CallbackSon proc 
	
	push {lr}
	
	;r�cup�ration des adresses des variables
	ldr R2,=Index
	ldr R3,[R2] ;R3 = Index
	
	;apr�s StartSon : comparaison avant �tude du son
    mov R1, #5511 ;R1 = 5511 = LongeurSon-1
	cmp R3, R1 ;regarde si R3 == R1 soit Index == LongueurSon-1  
	beq reset ;nous envoie � reset si R3 == R1	
	
	ldr R1,=Son ;R1=adresse premi�re composante de Son
	
	;r�cup�ration de la valeur du son dans r0
	ldrsh R0,[R1,R3,lsl#1] ;s pour sign� (copie du signe), h pour short integer (copie sur 2octets), lsl#1 (d�calage � gauche de 1 bit) pour multiplier par 2 l'index
	add R3,#1 ;incremente index
	;IDEM � : ldrsh R0,[R1,R3]
			; add R3,#2

	;Remarque : l'adresse de la variable Son ne nous est plus utile
	;donc R1 peut �tre utilis�e pour faire des op�rations

	;normalisation 1
	;add R0,#32768
	;mov R1,#720
	;mul R0,R1
	;mov R1,#65535
	;udiv R0,R1

	;normalisation 2
	mov R1,#45
	mul R0,R1
	asr R0,#12
	add R0,#360
	
	;avant StartSon : comparaison apr�s �tude du son
	b end_callback

reset ;si R3 == R1 soit si le son a d�j� �t� jou� en entier 
	;avant StartSon :
	;mov R3,#0 ;remise � 0 de l'INDEX, on rejoue le son
	;apr�s StartSon :
	mov R0,#0 ;on envoie 0

end_callback
	str R3, [R2] ;enregistrement de la valeur de l'INDEX pour que les changements soient visibles

	;Remarque : l'adresse de la variable Index ne nous est plus utile
	;donc R2 peut �tre utilis�e pour faire des op�rations
	
	ldr R2, =SortieSon
	str R0, [R2] ;on met notre valeur du son normalis�e � l'adresse de SortieSon

	bl PWM_Set_Value_TIM3_Ch3

	pop {pc}
	endp
	;IDEM � : pop {lr} 
		; bx lr
		; endp
		

StartSon proc 
	
	push {lr}
	ldr R2,=Index
	ldr R3,[R2] ;R3 = Index
	mov R3,#0 ;remise � 0 de l'INDEX => on joue le son
	str R3, [R2] ;enregistrement de la valeur de l'INDEX pour que les changements soient visibles
	pop {pc}
	endp
		
END