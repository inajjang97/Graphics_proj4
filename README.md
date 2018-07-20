HW4 20161601 송인아

-	! , @, #, $ ,%,^,& : 메인 카메라 구도 바꾸기
-	메인 카메라 s,b 축소/확대 
-	정적 cctv S,B 축소/확대
-	메인 카메라 이동/회전: x(X), y(Y),z(Z), u(U),v(V),n(N), h,j,k,l
-	정적 cctv 회전: H,J,K,L
-	q : screen 그리기
-	Q : screen effect = on 
-	w : screen width num 증가
-	W: screen height num 증가
-	e : blind effect = on
-	E : blind width 변화
-	r : cartoon effect = on
-	t / T : cartoon lever 변화
-	+ :  모든 스팟 조명이 깜박거림 ina_light_effect on/off 
-	방향키: 5번조명(반짝이는 조명) 상하좌우로 움직이기
-	왼쪽 ctrl : screen effect on 상태에서 왼쪽 ctrl을 누르면 ina effect on (모든 물건들에 격자모양 구멍 생김)

   <기본 쉐이딩 효과>
1)	구현 O - Phong shading에 기반을 둔 쉐이딩 방법을 적용
2)	구현 O – 쉬프트 키와 마우스 오른쪽 버튼을 동시에 누르는 동안은 Gouraud Shading이 적용 됨.
3)	구현 O – 쉬프트/마우스 오른쪽 버튼에서 손을 떼면 다시 phong shading으로 돌아옴 

< 재미있는 조명 효과>
1)	구현 O 
-	e를 누르면 blind 효과를 껐다/켰다 할 수 있음
-	E를 누르면 blind의 촘촘함을 조절할 수 있음
2)	구현 O
•	스팟 조명 깜박이기 효과 : ‘+’ 키를 누르면 모든 스팟 조명이 깜박이는 효과가 나타남
•	5번 광원은 단위시간마다 색이 빠르게 변함
•	원을 그리며 움직이는 조명 (조명1)
< 재미있는 기하물체 효과 >
1)	구현 O
•	 Screen Effect 
-	버튼 q로 draw screen를 on/off 
-	Q로 screen effect on/off
-	‘w’ & ‘W’로 screen width num/height num 조절
-	
2)	구현 O
•	cartoon effect
-	r 버튼으로 on/off
-	t/T 버튼으로 cartoon level 지정

•	ina effect
-	screen effect on 상태에서 왼쪽 왼쪽 ctrl 키로 on/off
-	w/W로 격자무늬 간격 조절 가능
-	방안의 객체들이 모두 체크 무늬로 구멍 뚤힌 것과 같은 효과
		
