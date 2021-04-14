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

    print("")

    print("Will you be travelling with children?")
    print("  1) Yes \n", "  2) No", sep="")
    children = (input("> "))
    if children == "1":
        children = "True"
    else:
        children = "False"
    user_input.append(children)

    print("")

    print("Which season do you plan to travel in?")
    print("  1) Spring \n","  2) Summer \n","  3) Autumn \n","  4) Winter",sep="")
    season = (input("> "))
    user_input.append(season)

    print("")

    print("What climate do you prefer?")
    print("  1) Cold \n","  2) Cool \n","  3) Moderate \n","  4) Warm \n","  5) Hot",sep="")
    climate = (input("> "))
    user_input.append(climate)

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
