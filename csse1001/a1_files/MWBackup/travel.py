#<<<<<<< HEAD
#"""

#"""

#__author__ = ""
#__date__ = ""


#=======
#__author__ = ""
#__date__ = ""

#>>>>>>> 95f525b6174b4aba59f5ea9787a24ee45082e903
from destinations import Destinations



def dumpDestinations():
    for destination in Destinations().get_all():
        # Task 2+: Add comparison logic here
            # Task 1: The following code is example code, and
        #         should be deleted once you implement task 2.
        print("Name:", destination.get_name())
        print("Continent:", destination.get_continent())
        print("Kid Friendly:", destination.is_kid_friendly())
        print("Cost:", destination.get_cost())
        print("Crime:", destination.get_crime())

        print("Climate:", destination.get_climate())

        print("Season Factors:",
              destination.get_season_factor("spring"),
              destination.get_season_factor("summer"),
              destination.get_season_factor("autumn"),
              destination.get_season_factor("winter"))

        print("Scores:",
              destination.get_interest_score("sports"),
              destination.get_interest_score("wildlife"),
              destination.get_interest_score("nature"),
              destination.get_interest_score("historical"),
              destination.get_interest_score("cuisine"),
              destination.get_interest_score("adventure"),
              destination.get_interest_score("beach"))
        print("-" * 40)


# getUserInput - a function to obtain the user input to be calculated in program
def getUserInput():
    '''Function is used to get the users input and store their input values in a list for later use'''
    user_input = list()

    print("")

    print("Welcome to Travel Inspiration!")
    name = input("What is your name? ")
    user_input.append(name)
    print("Hi ", name, "!", sep="")


    print("")
    counter = 0
    while counter == 0:
        print("Which content would you like to travel to?")
        print("  1) Asia \n", "  2) Africa \n", "  3) North America \n","  4) South America \n", "  5) Europe \n",
              "  6) Oceania \n",
              "  7) Antarctica", sep="")
        user_continent = int(input("> "))
        if 1 <= user_continent <= 7:
            if user_continent == 1:
                user_input.append("asia")
            elif user_continent == 2:
                user_input.append("africa")
            elif user_continent == 3:
                user_input.append("north america")
            elif user_continent == 4:
                user_input.append("south america")
            elif user_continent == 5:
                user_input.append("europe")
            elif user_continent == 6:
                user_input.append("oceania")
            else:
                user_input.append("antarctica")

            counter += 1
        else:
            print("Invalid input, please try again")


    print("")

    print("What is money to you?")
    print("  $$$) No object \n", "  $$) Spendable, so long as I get value from doing so \n",
          "  $) Extremely important, I want to spend as little as possible", sep="")
    user_money = (input("> "))
    user_input.append(user_money)


    print("")

    print("How much crime is acceptable when you travel?")
    print("  1) Low \n", "  2) Average \n", "  3) High", sep="")
    crime_rate = int(input("> "))
    user_input.append(crime_rate)
    '''if crime_rate == 1:
        crime_rate = "low"
        user_input.append(crime_rate)
    elif crime_rate == 2:
        crime_rate = "average"
        user_input.append(crime_rate)
    elif crime_rate == 3:
        crime_rate = "high"
        user_input.append(crime_rate)'''

    print("")

    print("Will you be travelling with children?")
    print("  1) Yes \n", "  2) No", sep="")
    children = (input("> "))
    if children == "1":
        children = 4
    else:
        children = 5
    user_input.append(children)

    print("")

    print("Which season do you plan to travel in?")
    print("  1) Spring \n","  2) Summer \n","  3) Autumn \n","  4) Winter",sep="")
    season = int(input("> "))
    user_input.append(season)

    print("")

    print("What climate do you prefer?")
    print("  1) Cold \n","  2) Cool \n","  3) Moderate \n","  4) Warm \n","  5) Hot",sep="")
    climate = int(input("> "))
    if climate == 1:
        user_input.append("cold")
    elif climate == 2:
        user_input.append("cool")
    elif climate == 3:
        user_input.append("moderate")
    elif climate == 4:
        user_input.append("warm")
    elif climate ==5:
        user_input.append("hot")


    print("")

    print("Now we would like to ask you some questions about your interests,on a scale \n",
          "of -5 to 5. -5 indicates a strong dislike, whereas 5 indicates strong interest,\n",
          "and 0 indicates indifference.",sep="")

    print("")

    print("How much do you like sports? (-5 to 5)")
    sports = int(input("> "))
    user_input.append(sports)

    print("")

    print("How much do you like wildlife? (-5 to 5)")
    wildlife = int(input("> "))
    user_input.append(wildlife)

    print("")

    print("How much do you like nature? (-5 to 5)")
    nature = int(input("> "))
    user_input.append(nature)

    print("")

    print("How much do you like historical sites? (-5 to 5)")
    historical_sites = int(input("> "))
    user_input.append(historical_sites)
    print("")

    print("How much do you like fine dining? (-5 to 5)")
    dining = int(input("> "))
    user_input.append(dining)


    print("")

    print("How much do you like adventure activites? (-5 to 5)")
    adventure = int(input("> "))
    user_input.append(adventure)

    print("")

    print("How much do you like the beach? (-5 to 5)")
    beach = int(input("> "))
    user_input.append(beach)

    print("")

    print("Thank you for answering all our questions.  Your next travel destination is:")
    print("none")

    return user_input

def crimeAsInt(crimeStr):
    if crimeStr == "low":
        crimeInt = 1
    elif crimeStr == "average":
        crimeInt = 2
    elif crimeStr == "high":
        crimeInt = 3
    else:
        crimeInt = 0

    return crimeInt

def seasonAsString(seasonInt):
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




def main():
    matchFound = False
    seasonFactor = 0.0
    user_list = getUserInput()
    print(user_list)
    for destination in Destinations().get_all():
        # Task 1: Ask questions here
        """user_continent = user_list[1]
        user_money = user_list[2]
        crime_rate = user_list[3]
        user_children = user_list[4]
        user_season = user_list[5]
        user_climate = user_list[6]
        user_sports = user_list[7]
        user_wildlife = user_list[8]
        user_nature = user_list[9]
        user_historical = user_list[10]
        user_dining = user_list[11]
        user_adventure = user_list[12]
        user_beach = user_list[13]"""
        crime_rate = user_list[3]
        #print("User choice", user_list[1], "destination's continent", destination.get_continent())
        if user_list[1] == destination.get_continent():
            #print("Continent matched")
            if len(user_list[2]) >= len(destination.get_cost()):
               # print("Price Matched")
                if (user_list[4]) >= len(str(((destination.is_kid_friendly())))):
                    #print("Kid match")
                    if (crime_rate) >= crimeAsInt(destination.get_crime()):
                       # print("crime match")
                       #Task 3 - finding the climate and season factor
                       if user_list[6] == destination.get_climate():

                           if seasonFactor < destination.get_season_factor(seasonAsString(user_list[5])):
                                seasonFactor = destination.get_season_factor(seasonAsString(user_list[5]))
                                lastDestination = destination
                                matchFound = True
                                interest_score = user_list[7] * destination.get_interest_score('sports')
                                + user_list[8] * destination.get_interest_score('wildlife')
                                + user_list[9] * destination.get_interest_score('nature')
                                + user_list[10] * destination.get_interest_score('historical')
                                + user_list[11] * destination.get_interest_score('cuisine')
                                + user_list[12] * destination.get_interest_score('adventure')
                                + user_list[13] * destination.get_interest_score('beach')
                                score = seasonFactor * interest_score

    # After searching through destinations, print the output
    if matchFound == True:
        print ("The available option is", lastDestination.get_name())
        print (score)
    else:
        print("None")





if __name__ == "__main__":

    main()

# findMatch - a function to find an appropriate country for the user to travel to based on their input
# def findMatch():
# main - the main function for the Travel program



