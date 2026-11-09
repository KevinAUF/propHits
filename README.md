PropHits - Group 37 (GatorBoys)

Members: Aiden Higgs, Brennan Beaulieu, Kevin Astudillo Uribe

Video: https://youtu.be/JGwJ9Xi6QCs


WHAT IS THIS?

PropHits is an NFL player prop analyzer we made for our DSA Project 2. You type in a prop, like Justin Jefferson over 75.5 receiving yards, and the program tells you the chance that prop hits and what the fair odds should be.

You can use this to check if a sportsbook is giving you a good price. If the book's odds are better than our fair odds, it's a good bet. If they're worse, skip it.


DATA STRUCTURES

We built two data structures from scratch and compare them.

The first one is a Hash Table. It stores every player's name as the key and all their past games as the value. Lookups are O(1) on average.

The second one is a B+ Tree. It stores the same player data, but the leaves are linked together, so we can search for every name that starts with certain letters. We use this for the "Did you mean" suggestions when you type a name wrong. Lookups are O(log n).

We also have a Defense Table that keeps track of how many yards each team allows to each position. This one is just a helper and isn't part of the comparison.

Neither the hash table nor the B+ tree uses std::unordered_map or std::map.


HOW THE MODEL WORKS

1. We take the player's last 17 games (he needs at least 3 games).
2. We find his average and standard deviation for the stat you picked.
3. If you enter an opponent, we adjust his average based on that defense. For example, if Dallas allows 10% more receiving yards to WRs than the league average, his average goes up 10%.
4. We treat the stat like a normal distribution (bell curve) and find the chance he goes over or under the line.
5. We turn that percentage into American odds. If the chance is 50% or more, the odds are -100 x p / (1 - p). If it's under 50%, the odds are +100 x (1 - p) / p.

You can pick from four stats: passing yards, rushing yards, receiving yards, and receptions.


DATA

All of our data is free from nflverse (https://github.com/nflverse/nflverse-data/releases).

Make a folder called data in the main project folder and put these 4 files in it: player_stats.csv (games from 1999 to 2024), stats_player_week_2025.csv (the 2025 season), stats_player_week_2026.csv (the 2026 season), and games.csv (schedules and matchups).


HOW TO RUN IT

We used CLion.

1. Open the propHits folder in CLion.
2. Make sure the data folder is in the main project folder, right next to cmake-build-debug.
3. Hit the green run button.

The program looks for the data in ../data, so it has to run from inside cmake-build-debug. CLion does this by default. If it says "No rows loaded. Check the data folder." then the data folder is in the wrong place.


HOW TO USE IT

When it starts, it shows how many games and players were loaded, and then the menu:

1. Analyze a prop
2. Benchmark Hash Table vs B+ Tree
3. Quit

If you pick 1 (Analyze a prop), it asks you five things one at a time.

Player: Type the name, like Justin Jefferson. Capital letters don't matter. If you only type part of the name, like Justin J, it gives you up to 5 suggestions.

Stat: Type 1 for pass yds, 2 for rush yds, 3 for rec yds, or 4 for receptions.

Over or Under: Type O or U.

Line: Type the number, like 75.5. It has to be positive.

Opponent: Type the team code, like DAL or KC. You can leave this blank if you don't want the defense adjustment.

After that it prints the chance, the fair odds, the base percentage, how much the defense changed it, how many of his last 17 games went over or under, and how long the lookup took in the hash table vs the B+ tree.

If you pick 2 (Benchmark), it times inserting and searching in both data structures using all of the data, so you can see which one is faster.

If you pick 3, the program closes.

If you type something wrong, like a stat number that doesn't exist, a negative line, or a fake team, it asks you again instead of crashing.


FILES

main.cpp has the menu and prints the prop results. GameRow.h holds one game for one player. DataLoader.h and DataLoader.cpp read the CSV files. HashTable.h is our hash table, and Bp.h and Bp.cpp are our B+ tree. DefenseTable.h figures out the defense adjustment. PropModel.h does the math for the percentage and the odds. Benchmark.h times the hash table vs the B+ tree.


WHAT WE WANT TO ADD NEXT

We want to add weather (wind, temperature, dome or outdoor), home vs away games, and touchdown props. We also want to test the model on old seasons to see how accurate it is. Eventually we want to turn it into a real app that compares our odds with real sportsbooks.
