
"""
 _____                   _   _____                _           _   _
|_   _|                 | | |_   _|              (_)         | | (_)
  | |_ __ __ ___   _____| |   | | _ __  ___ _ __  _ _ __ __ _| |_ _  ___  _ __
  | | '__/ _` \ \ / / _ \ |   | || '_ \/ __| '_ \| | '__/ _` | __| |/ _ \| '_ \
  | | | | (_| |\ V /  __/ |  _| || | | \__ \ |_) | | | | (_| | |_| | (_) | | | |
  \_/_|  \__,_| \_/ \___|_|  \___/_| |_|___/ .__/|_|_|  \__,_|\__|_|\___/|_| |_|
                                           | |
                                           |_|
__file_name__ = Travel.py
__desription__ = main source file for CCSE Assignment 1
__author__ = Ryan Ward
__date__ = 29/3/19

"""



from destinations import Destinations

#declare user input class type
class userInput:
    """The class system will be used to store the user's input into their associated variables, i.e. the user's choice
    of continents will be assigned to the continents variable.
    """
    name = ""
    continents = [] #appends input to a list for multiple inputs
    money = ""
    crime_rate = 0
    children = False
    seasons = [] #appends input to a list for multiple inputs
    climate = ""
    sports = 0
    wildlife = 0
    nature = 0
    historical = 0
    dining = 0
    adventure = 0
    beach = 0

#define const for invalid input
INVALID_INPUT = -1000

def getValidInputRange( strInput, low, high):
    """Gets the input range for testing valid inputs
    Parameters:
        strInput: The input of the user
        low: the lowest value for the given variable
        high: the highest value for the given variable
    Return:
        If the input is valid or not"""
    validInput = INVALID_INPUT
    try:
        inpVal = int(strInput)
        if(inpVal >= low and inpVal <= high):
            validInput = inpVal
        else:
            validInput = INVALID_INPUT

    except ValueError:
        validInput = INVALID_INPUT
    return validInput

def isValidMoney(money):
    """Finds the valid input for the money variable.  Makes sure that the only symbol entered is the "$" symbol
    Parameters:
        money: the user's input from the question
    Return:
        If the user input value is valid"""
    for i in range(0, len(money)):
        if (money[i] != '$'):
            return False

    return True

def crimeValue(crimeStr):
    """Find the crime parameters in the .csv file and converts them from strings (str) to integers (int) for later comparison"""
    if crimeStr == "low":
        crimeInt = 1
    elif crimeStr == "average":
        crimeInt = 2
    elif crimeStr == "high":
        crimeInt = 3
    else:
        crimeInt = 0

    return crimeInt

def seasonName(seasonInt):
    """Takes the season parameters from the .csv file and converts them from strings (str) to integers (int) for later
    comparison
    Parameters:
         Takes the user input from above and converts ths integer into a string
    Return:
        Returns the chosen season a string"""
    if seasonInt == 1:
        seasonStr = "spring"
    elif seasonInt == 2:
        seasonStr = "summer"
    elif seasonInt == 3:
        seasonStr = "autumn"
    elif seasonInt == 4:
        seasonStr = "winter"
    else:
        seasonStr = ""

    return seasonStr

def getInterestScore(finalInput, userDestination):
    """Finds the score for the final destinations
    Parameters:
        user_input: calls the the user input to be calculated
        destination: calls the destination into the equation to calculate total score
    Return:
        outputs the score calculated by the equation"""
    user_input = finalInput
    destination = userDestination

    interestScore = user_input.sports * destination.get_interest_score('sports') \
    + user_input.wildlife * destination.get_interest_score('wildlife') \
    + user_input.nature * destination.get_interest_score('nature') \
    + user_input.historical * destination.get_interest_score('historical') \
    + user_input.dining * destination.get_interest_score('cuisine') \
    + user_input.adventure * destination.get_interest_score('adventure') \
    + user_input.beach * destination.get_interest_score('beach')

    return interestScore

def safeForKids(haveKids, kidFriendly):
    """Determines if the user is travelling with kids or not, as if the country is kid friendly, anyone can go, but
    if it is no kid friendly, then only people travelling without kids can go there
    Parameters:
          haveKids: The user's input as to if they have kids or not
          kidFriendly: the .csv info on if the country if kid friendly
    Return:
        bool: Uses boolean (bool) to set the user input to true or false to compare with the .csv file"""
    isSafeForKids = True

    if(haveKids and not kidFriendly):
        isSafeForKids = False

    return isSafeForKids

def getContinents(input_list, user_input):
    """Allows for multiple inputs for the continents to be accepted
    Parameters:
        input_list: a list of the integers the user has given
        user_input: appends the string of the user's input
    Returns:
        (str) The multiples inputs as strings, and whether the input is valid"""
    input_list = input_list.strip() # remove spaces
    num_list = input_list.split(',') # extract each number into list
    #num_list = set(num_list) # remove duplicates from list

    for x in range(len(num_list)):
        numStr = num_list[x]
        numStr = numStr.strip()
        if(numStr.isdigit()):
            num = int(numStr)

            # ensure number is within valid range
            if (num >= 1 and num <= 7):

                # assign to corresponding continent name
                if num == 1:
                    user_input.continents.append("asia")
                elif num == 2:
                    user_input.continents.append("africa")
                elif num == 3:
                    user_input.continents.append("north america")
                elif num == 4:
                    user_input.continents.append("south america")
                elif num == 5:
                    user_input.continents.append("europe")
                elif num == 6:
                    user_input.continents.append("oceania")
                else:
                    user_input.continents.append("antarctica")

                validInput = True

            else:
                validInput = False

        else:
            validInput = False

    return validInput

def getSeasons(input_list, user_input):
    """Allows for multiple inputs for the seasons to be accepted
        Parameters:
            input_list: a list of the integers (int) the user has given
            user_input: appends the string (str) of the user's input
        Returns:
            (bool) The multiples inputs as booleans, and whether the input is valid"""
    input_list = input_list.strip()  # remove spaces
    num_list = input_list.split(',')  # extract each number into list
    for x in range(len(num_list)):
        numStr =  num_list[x]
        numStr =  numStr.strip()
        if(numStr.isdigit()):
            num = int(numStr)

            # ensure number is within valid range
            if (num >= 1 and num <= 4):
                user_input.seasons.append(num)
                validInput = True
            else:
                validInput = False
        else:
            validInput= False

    return validInput

# getUserInput - a function to obtain the user input to be calculated in program
def getUserInput():
    """getUserInput is used to get the users input and store their input values in a list for later use.
    The user will be asked a question and they will be required to type a response, which will then be added to the class
    variable.
    Parameters:
        user_input: the dictionary in which all user input is appended to
    Returns:
        (dict) Dictionary with all the inputted values"""

    user_input = userInput()
    print("Welcome to Travel Inspiration!")
    print("")
    user_input.name = input("What is your name? ")
    print("")

    print("Hi, ", user_input.name,"!",sep='')
    print("")
    # Find the user's input for their preferred destination
    counter = 0
    while counter == 0:
        print("Which continents would you like to travel to?")
        print("  1) Asia\n", "  2) Africa\n", "  3) North America\n","  4) South America\n", "  5) Europe\n",
              "  6) Oceania\n",
              "  7) Antarctica", sep = "")
        user_continent = input("> ")
        if (getContinents(user_continent, user_input) == True):
            counter += 1

             #Use a simple counter system to break the loop - the break command was not used, as it would be
            #breaking in the middle of a loop
        else:
            print("")
            print("I'm sorry, but " + str(user_continent) + " is not a valid choice. Please try again.")
            print("")
            #Generic invalid statement used for all input statements


    print("")
    counter = 0 #counter systems will be used for the first half of the user input section, as the first half is slightly more complex
    #than the simple -5 to 5 input of the second section.
    while counter == 0:
        print("What is money to you?")
        print("  $$$) No object\n", "  $$) Spendable, so long as I get value from doing so\n",
          "  $) Extremely important; I want to spend as little as possible", sep="")

        user_money = input("> ")
        if isValidMoney(user_money):
            user_input.money = user_money
            counter += 1
        else:
            print("")
            print("I'm sorry, but " + str(user_money) + " is not a valid choice. Please try again.")
            print("")


    print("")
    counter = 0
    while counter == 0:
        print("How much crime is acceptable when you travel?")
        print("  1) Low\n", "  2) Average\n", "  3) High", sep="")
        crime_rate = (input("> "))

        if  getValidInputRange(crime_rate, 1, 3) != INVALID_INPUT:
            user_input.crime_rate = int(crime_rate)
            counter += 1
        else:
            print("")
            print("I'm sorry, but " + str(crime_rate) + " is not a valid choice. Please try again.")
            print("")

    print("")
    counter = 0
    while counter ==0:
        print("Will you be travelling with children?")
        print("  1) Yes\n", "  2) No", sep="")
        children = (input("> "))
        if children == "1":
            user_input.children = True
            counter += 1
        elif children == "2":
            user_input.children = False
            counter += 1
        else:
            print("")
            print("I'm sorry, but " + str(children) + " is not a valid choice. Please try again.")
            print("")

    print("")
    counter = 0
    while counter == 0:
        print("Which seasons do you plan to travel in?")
        print("  1) Spring\n","  2) Summer\n","  3) Autumn\n","  4) Winter",sep="")
        seasons = (input("> "))
        if(getSeasons(seasons, user_input) == True):

            counter += 1
        else:
            print("")
            print("I'm sorry, but " + str(seasons) + " is not a valid choice. Please try again.")
            print("")


    print("")
    counter = 0
    while counter == 0:
        print("What climate do you prefer?")
        print("  1) Cold\n","  2) Cool\n","  3) Moderate\n","  4) Warm\n","  5) Hot",sep="")
        climate = (input("> "))
        if getValidInputRange(climate, 1, 5) != INVALID_INPUT:
            if int(climate) == 1:
                user_input.climate = "cold"
                counter += 1
            elif int(climate) == 2:
                user_input.climate = "cool"
                counter += 1
            elif int(climate) == 3:
                user_input.climate = "moderate"
                counter += 1
            elif int(climate) == 4:
                user_input.climate = "warm"
                counter += 1
            elif int(climate) ==5:
                user_input.climate = "hot"
                counter += 1
        else:
                print("")
                print("I'm sorry, but " + str(climate) + " is not a valid choice. Please try again.")
                print("")

    print("")

    print("Now we would like to ask you some questions about your interests, on a scale of -5 to 5. -5 indicates strong dislike, whereas 5 indicates strong interest, "
          "and 0 indicates indifference.",sep="")
    #gets the user's interest in sports
    print("")
    while True:
        print("How much do you like sports? (-5 to 5)")
        sports = (input("> "))
        if getValidInputRange(sports, -5, 5) != INVALID_INPUT:
            sports = int(sports)
            user_input.sports = sports
            break
        else:
            print("")
            print("I'm sorry, but " + str(sports) + " is not a valid choice. Please try again.")
            print("")
        #as there is only one test to find validity of the input, a break statement was used.

    # gets the user's interest in wildlife
    while True:
        print("")

        print("How much do you like wildlife? (-5 to 5)")
        wildlife = (input("> "))
        if getValidInputRange(wildlife, -5, 5) != INVALID_INPUT:
            wildlife = int(wildlife)
            user_input.wildlife = wildlife
            break
        else:
            print("")
            print("I'm sorry, but " + str(wildlife) + " is not a valid choice. Please try again.")
            print("")
    # gets the user's interest in nature
    print("")
    while True:
        print("How much do you like nature? (-5 to 5)")
        nature = int(input("> "))
        if getValidInputRange(nature, -5, 5) != INVALID_INPUT:
            nature = int(nature)
            user_input.nature = nature
            break

        else:
            print("")
            print("I'm sorry, but " + str(nature) + " is not a valid choice. Please try again.")
            print("")

    # gets the user's interest in historical sites
    print("")
    while True:
        print("How much do you like historical sites? (-5 to 5)")
        historical_sites = (input("> "))
        if getValidInputRange(historical_sites, -5, 5) != INVALID_INPUT:
            historical_sites = int(historical_sites)
            user_input.historical = historical_sites
            break
        else:
            print("")
            print("I'm sorry, but " + str(historical_sites) + " is not a valid choice. Please try again.")
            print("")

    # gets the user's interest in cuisine
    print("")
    while True:
        print("How much do you like fine dining? (-5 to 5)")
        dining = (input("> "))
        if getValidInputRange(dining, -5, 5) != INVALID_INPUT:
            dining = int(dining)
            user_input.dining = dining
            break
        else:
            print("")
            print("I'm sorry, but " + str(dining) + " is not a valid choice. Please try again.")
            print("")

    # gets the user's interest in adventure activities
    print("")
    while True:
        print("How much do you like adventure activities? (-5 to 5)")
        adventure = (input("> "))
        if getValidInputRange(adventure, -5, 5) != INVALID_INPUT:
            adventure = int(adventure)
            user_input.adventure = adventure
            break
        else:
            print("")
            print("I'm sorry, but " + str(adventure) + " is not a valid choice. Please try again.")
            print("")

    # gets the user's interest in the beach
    print("")
    while True:
        print("How much do you like the beach? (-5 to 5)")
        beach = (input("> "))
        if getValidInputRange(beach, -5, 5) != INVALID_INPUT:
            beach = int(beach)
            user_input.beach = beach
            break
        else:
            print("")
            print("I'm sorry, but " + str(beach) + " is not a valid choice. Please try again.")
            print("")


    print("")




    return user_input

def main():
    """Main function where all the logic is processed, imports all the class variables and uses multiple
    nested if statements to find the final country
    Parameters:
        Takes all user input and passes them through various logic statements to find final country
    Return:
          The name of the country that the user is best suited to"""
    matchFound = False
    score = INVALID_INPUT #score was originally set to = 0, however, some scores can be initially less that 0, therefore, it is set to a large negative number
    #(see score calculation - line 363)
    user_input = getUserInput()
    for destination in Destinations().get_all():
        crime_rate = user_input.crime_rate
        # if the continents(s) selected are in the .csv file
        if destination.get_continent() in user_input.continents:
            # if the preferred choice of spending matches with the continent
            if len(user_input.money) >= len(destination.get_cost()):
                #if the the country is child friendly according to the user's preference
                if safeForKids(user_input.children, destination.is_kid_friendly()):
                    #if the crime rate of the country is less than or equal to the preferred crime rate
                    if crime_rate >= crimeValue(destination.get_crime()):

                       #Task 3 - finding the climate and season factor using the above functions
                       if user_input.climate == destination.get_climate():
                           for season in user_input.seasons:
                                #finds the season factor from all of the remaining destinations
                               destSeasonFactor = destination.get_season_factor(seasonName(season))

                               interest_score = getInterestScore(user_input, destination)
                               #multiplies the destination's season factor by the user's interest score
                               destScore = destSeasonFactor * interest_score
                               # finds the country with the largest score and breaks the loop to print one option
                               if score < destScore:
                                   score = destScore
                                   lastDestination = destination
                                   matchFound = True

    # After searching through destinations, print the output
    if matchFound == True:
       print ("Thank you for answering all our questions. Your next travel destination is:\n",
              lastDestination.get_name(),sep="")

    else:
       print("Thank you for answering all our questions. Your next travel destination is: None")




if __name__ == "__main__":

    main()




