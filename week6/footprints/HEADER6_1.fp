
	
	
Element(0x00 "Header connector, DIP pin numbering" "" "HEADER6_1" 260 0 3 100 0x00)
(
    Pad(50 50 -20 50 60 "1" 0x101)
    Pad(50 150 -20 150 60 "2" 0x101)
    Pad(50 250 -20 250 60 "3" 0x101)

    Pad(150 50 220 50 60 "4" 0x101)
    Pad(150 150 220 150 60 "5" 0x101)
    Pad(150 250 220 250 60 "6" 0x101)

	#Pin(20 50 60 0 "1" 0x101)
	#Pin(20 150 60 0 "2" 0x101)
	#Pin(20 250 60 0 "3" 0x101)
	
	#Pin(180 250 60 0 "4" 0x101)
	#Pin(180 150 60 0 "5" 0x101)
	#Pin(180 50 60 0 "6" 0x101)
	
	ElementLine(0 0 0 300 10)
	ElementLine(0 300 200 300 10)
	ElementLine(200 300 200 0 10)
	ElementLine(200 0 0 0 10)
	ElementLine(0 100 100 100 10)
	ElementLine(100 100 100 0 10)
	Mark(50 50)
)

