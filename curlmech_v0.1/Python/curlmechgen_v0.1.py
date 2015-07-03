# Curl Mechanism Generator v 0.1
# -------------------------------
# Generates 4 parameters for the flexible piece,
# and 3 parameters for the code

import json

antimony_file = "flexible_piece_template.sb"
echo_input = False
prompt = "> "

dot_location = 99
dot_position = 99
bot_nature = 99
bot_name = ""

length = 99
rib_r = 99
rib_m = 99
rib_l = 99
motor_speed = 999
motor_reel_in = 999
motor_reel_out = 999

def askDotLocation():
	global dot_location

	print "What dot will the sensor be located on? (3, 4, 5, 6)"
	dot_location = raw_input(prompt)

	if not dot_location.isdigit():
		print "Integers only please"
		askDotLocation()

	dot_location = int(dot_location)

	if echo_input:
		print "\nInput: %d\n" % (dot_location)

	if dot_location < 3 or dot_location >= 7:
		print "Location unavailable at this time, please choose a different dot location\n"
		askDotLocation()

def askDotPosition():
	global dot_position

	print "Will the sensor position be on the 1- right, 2- middle, or 3- left?"
	dot_position = raw_input(prompt)

	if not dot_position.isdigit():
		print "Integers only please"
		askDotPosition()

	dot_position = int(dot_position)

	if echo_input:
		print "\nInput: %d\n" % (dot_position)

	if dot_position <= 0 or dot_position > 3:
		print "Position unavailable at this time, please choose a different dot position"
		askDotPosition()

def askBotNature():
	global bot_nature

	print "Is this robot catepillar 1- calm or 2- energetic?"
	bot_nature = raw_input(prompt)

	if not bot_nature.isdigit():
		print "Integers only please"
		askBotNature()

	bot_nature = int(bot_nature)

	if echo_input:
		print "\nInput: %d\n" % (bot_nature)

	if bot_nature < 1 or bot_nature > 2:
		print "Nature unavailable at this time, please choose a different nature"
		askBotNature()

def askBotName():
	global bot_name

	print "Last question- what is your robot catepillar's nickname?"
	bot_name = raw_input(prompt)

	if echo_input:
		print "\nInput: %s\n" % (bot_name)

def calculateLength():
	global length

	if dot_position == 2:
		length = (dot_location*10)-20

	elif dot_position == 1 or dot_position == 3:
		length = (dot_location*10)-10

def calculateRibs():
	global rib_r
	global rib_m
	global rib_l

	if dot_position == 2:
		if dot_location == 3:
			rib_r = rib_m = rib_l = 2
		elif dot_location >= 4 and dot_location < 6:
			rib_r = rib_m = rib_l = 4
		elif dot_location >= 6:
			rib_r = rib_m = rib_l = 6

	elif dot_position == 1: # right
		rib_r = 2

		if dot_location == 3:
			rib_m = rib_l = 4
		elif dot_location >= 4 and dot_location < 6:
			rib_m = rib_l = 4
		elif dot_location >= 6:
			rib_m = rib_l = 6

	elif dot_position == 3: # left
		rib_l = 2

		if dot_location == 3:
			rib_r = rib_m = 4
		elif dot_location >= 4 and dot_location < 6:
			rib_r = rib_m = 4
		elif dot_location >= 6:
			rib_r = rib_m = 6

	if dot_location == 6 and bot_nature == 2:
		rib_r = rib_l = 4
		rib_m = 8 #this will be fun!

def calculateSpeeds():
	global motor_speed
	global motor_reel_in
	global motor_reel_out

	if bot_nature == 1:
		motor_speed = 128 + ((dot_location-2)*5)
	elif bot_nature == 2:
		motor_speed = 192 + ((dot_location-2)*7)

	if dot_location == 3:
		motor_reel_in = 10
		motor_reel_out = 20
	elif dot_location >= 4 and dot_location < 6:
		motor_reel_in = 50
		motor_reel_out = 15
	elif dot_location >= 6:
		motor_reel_in = 100
		motor_reel_out = 10

def generateSbModel():

	with open(antimony_file, "r+") as json_file:

		json_data = json.load(json_file)

		#print(json_data['connections'])
		#print(len(json_data['nodes']))

		nodes = json_data['nodes']
		name = "-"
		dim_index = 99

		for j in range(0, len(nodes)):
			datums = nodes[j]['datums']

			for i in range(0, len(datums)):
				meep = datums[i]
				if meep['name'] == "__name":
					name = meep['expr']
					#print(name)

					if name == "dim":
						dim_index = j
					
					break


		dim_datum = nodes[dim_index]['datums']

		for i in range(0, len(dim_datum)):
			meep = dim_datum[i]

			# it seems like all the variables in the .sb file
			# for expr are strings, so just going to set 
			# them all as strings too

			if meep['name'] == "length":
				json_data['nodes'][dim_index]['datums'][i]['expr'] = str(length) #("%s" % (length))

			if meep['name'] == "rib_l":
				json_data['nodes'][dim_index]['datums'][i]['expr'] = str(rib_l)

			if meep['name'] == "rib_m":
				json_data['nodes'][dim_index]['datums'][i]['expr'] = str(rib_m)

			if meep['name'] == "rib_r":
				json_data['nodes'][dim_index]['datums'][i]['expr'] = str(rib_r)


		json_file.seek(0)
		json.dump(json_data, json_file, indent=4)




print "Welcome to the robot catepillar curl mechanism generator v 0.1!\n\n"

askDotLocation()
askDotPosition()
askBotNature()
askBotName()

print "Generating %s's parameters...\n\n" % (bot_name)

calculateLength()
calculateRibs()
calculateSpeeds()

print "Flexible piece parameters:"
print "--------------------------"
print "Length: %d" % (length)
print "Rib R: %d" % (rib_r)
print "Rib M: %d" % (rib_m)
print "Rib L: %d" % (rib_l)

print "\n"

print "Code parameters:"
print "----------------"
print "Motor speed: %d" % (motor_speed)
print "Motor reel in: %d" % (motor_reel_in)
print "Motor reel out: %d" % (motor_reel_out)
print "Optional comment: // my nickname is %s nice to meet you! *\[ ^o^ ]/*" % (bot_name) 

print "\n"

print "Robot's recipe:"
print "---------------"
print "%d-%d-%d:%d-%d-%d-%d:%d-%d-%d:%s" % (dot_location, dot_position, bot_nature, length, rib_l, rib_m, rib_r, motor_speed, motor_reel_in, motor_reel_out, bot_name)

print "\n"

generateSbModel()

print "Generated Antimony model: %s" % (antimony_file)

