
	
	
Element(0x00 "Jumper, i.e. single row headers" "" "JUMPER6" 160 0 3 100 0x00)
(

     Pad(50 50 -20 50 60 "1" 0x101)
     Pad(50 150 -20 150 60 "2" 0x101)
     Pad(50 250 -20 250 60 "3" 0x101)
     Pad(50 350 -20 350 60 "4" 0x101)
     Pad(50 450 -20 450 60 "5" 0x101)
     Pad(50 550 -20 550 60 "6" 0x101)

	 #Pin(50 50 60 38 "1" 0x101)
	 #Pin(50 150 60 38 "2" 0x01)
	 #Pin(50 250 60 38 "3" 0x01)
	 #Pin(50 350 60 38 "4" 0x01)
	 #Pin(50 450 60 38 "5" 0x01)
	 #Pin(50 550 60 38 "6" 0x01)
	 
	ElementLine(0 0 0 600 10)
	ElementLine(0 600 100 600 10)
	ElementLine(100 600 100 0 10)
	ElementLine(100 0 0 0 10)
	ElementLine(0 100 100 100 10)
	ElementLine(100 100 100 0 10)
	Mark(50 50)
)

