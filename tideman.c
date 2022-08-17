#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// degCandidate - Degree of Candidate - from Degree of Vertex(the number of edges that depart from the vertex).
typedef struct
{
    //Candidate number.
    int number;
    // The number of ribs that depart from the vertex of the candidate.
    int outgoingEdges;
}
degCandidate;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count = 0;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(int arr_begin,int arr_end);
void lock_pairs(void);
void print_winner(void);
bool findLoop(int vertex);
void dfsForLoop(int current_vertex,int color[], bool *loop,bool adjacency_matrix[][MAX]);
void sort_candidatesDeg(int arr_begin, int arr_end, degCandidate candidatesDeg[]);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    // Fill the matrix of preferences with 0;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            preferences[i][j] = 0;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");
    // printf("Number of voters: ");
    // scanf("%i", &voter_count);
    // printf("\n");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);
            // char *name = malloc(2);
            // printf("Rank %i: ", j + 1);
            // scanf("%s", name);
            // printf("\n");

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");

        // for(int j = 0; j < candidate_count; j++)
        // {
        //     printf("%d\n", ranks[j]);
        // }
    }
    // for (int i = 0; i < candidate_count; i++)
    // {
    //     for (int j = 0; j < candidate_count; j++)
    //     {
    //         printf("%d ", preferences[i][j]);
    //     }
    //     printf("\n");
    // }




    add_pairs();



    //Shows the matrix preferences[i][j]
    for (int i = 0; i < candidate_count + 2; i++)
    {
        if (i == 0)
        {
            for (int j = 0; j < candidate_count + 2; j++)
            {
                if (j < 2)
                {
                    printf(" ");
                }
                else
                {
                    printf("%i ", (j - 2));
                }
            }
            // printf("\n");

        }

        else if (i == 1)
        {
            for (int j = 0; j < candidate_count + 2; j++)
            {
                if(j < 2)
                {
                    printf(" ");
                }
                else
                {
                    printf("- ");
                }
            }
            // printf("\n");
        }
        else
        {
            for (int j = 0; j < candidate_count + 1; j++)
            {
                if(j == 0)
                {
                    printf("%i|", (i - 2));
                }
                else
                {
                    printf("%d ", preferences[i - 2][j - 1]);
                }
            }
        }
            printf("\n");
    }
    printf("\n");

    // Shows the winner and loser of the struct pair(pairs)
    for (int i = 0; i < pair_count; i++)
        {
            printf("pair: %d\nwin: %d\nlos: %d\n\n", i, pairs[i].winner, pairs[i].loser);
        }

    /*
        Shows the strength of each pair,
        in the difference of votes between the winner and the loser.
    */
    for(int i = 0; i < pair_count; i++)
    {
        printf("pair strength %i: %i\n", i, (preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner]));
    }
    printf("\n");


    sort_pairs(0, pair_count - 1);

    lock_pairs();

    // Shows the winner and loser of the struct pair(pairs)
    for (int i = 0; i < pair_count; i++)
        {
            printf("pair: %d\nwin: %d\nlos: %d\n\n", i, pairs[i].winner, pairs[i].loser);
        }

    /*
        Shows the strength of each pair,
        in the difference of votes between the winner and the loser.
    */
    for(int i = 0; i < pair_count; i++)
    {
        printf("pair strength %i: %i\n", i, (preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner]));
    }
    printf("\n");

        //Shows the matrix locked
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if(locked[i][j])
            {
                printf("T ");
            }
            else
            {
                printf("F ");
            }
        }
        printf("\n");
    }
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Candidate index in arrey canditates[]
    int candidates_num;

    for (int i = 0; i < candidate_count; i++)
    {
        if(!strcmp(name, candidates[i]))
        {
            candidates_num = i;
            break;
        }

        //If the name of the candidate is not found in array of c. even in last iteration of the cycle, the function will return folse
        if(i == candidate_count - 1)
        {
            return false;
        }
    }
    for (int i = 0; i < rank; i++)
    {
        if(candidates_num == ranks[i])
        {
            return false;
        }

    }

    ranks[rank] = candidates_num;
    return true;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        int row = ranks[i];
        for (int j = i + 1; j < candidate_count; j++)
        {
            int column = ranks[j];
            preferences[row][column] += 1;
        }
    }
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    //compare all pairs of candidates
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // compare the number of votes for one or another candidate in pairs
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count += 1;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count += 1;
            }
        }
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(int arr_begin,int arr_end)
{
    // int arr_begin = 0;
    // int arr_end = sizeof(pairs) / sizeof(pairs[0]);
    //Turn the array so that its beginning is 1.
    //b - begin of array
    int b = arr_begin + 1;
    //e - end of array
    int e = arr_end + 1;
    int b2 = b - 1;


    if (b == e)
    //e - b + 1 == 1
    {
    }
    else if (b < e)
    {
        /*
            n - numer of array elements;
            n_n - next number(half of the current array);
            r - reminder;

            l_n_b - left next begin(the beginning of the left part of the array);
            l_n_e - left next end(the ending of the left part of the array);

            r_n_b - right next begin(the beginning of the right part of the array);
            r_n_e - right next end(the ending of the right part of the array);

        */
        int n, n_n, r, l_n_e, l_n_b, r_n_b, r_n_e;

        n = e - b + 1;
        n_n = n / 2;
        r = n % 2;

        l_n_e = b + n_n + r - 1 - 1;
        l_n_b = b - 1;

        r_n_b = e - n_n + 1 - 1;
        r_n_e = e - 1;

        //Sorting the left half of the array.
        sort_pairs(l_n_b, l_n_e);
        //Sorting the right half of the array.
        sort_pairs(r_n_b, r_n_e);

        pair newPairsArrey[n];

        //Marge sorted halves of the array.
        for (int i = 0; i < n; i++)
        {

            if(r_n_b > r_n_e)
            {
                newPairsArrey[i] = pairs[l_n_b];
                 //Go to the next number in the right part.
                l_n_b += 1;
                continue;
            }
            else if(l_n_b > l_n_e)
            {
                newPairsArrey[i] = pairs[r_n_b];
                //Go to the next number in the left part.
                r_n_b +=1;
                continue;
            }

            int strongOfLeftPair = (preferences[pairs[l_n_b].winner][pairs[l_n_b].loser] - preferences[pairs[l_n_b].loser][pairs[l_n_b].winner]);
            int strongOfRightPair = (preferences[pairs[r_n_b].winner][pairs[r_n_b].loser] - preferences[pairs[r_n_b].loser][pairs[r_n_b].winner]);

            if (strongOfLeftPair < strongOfRightPair)
            {
                /*
                    If the number in the left half is smaller then the number in the right half.
                    And the left half is not passed(begin in not biger than the end of left half).
                    Write the left number in the array to for already sorted numbers.
                */

                //If the left half is already passed. Wirte the remaining number in the right half.

                newPairsArrey[i] = pairs[r_n_b];
                r_n_b +=1;

            }
            else if (strongOfLeftPair > strongOfRightPair)
            {
                /*
                    If the number in the right half is smaller then the number in the left half.
                    And the right half is not passed(begin in not biger than the end of right half).
                    Write the left number in the array to for already sorted numbers.
                */

                //If the right half is already passed. Wirte the remaining number in the left half.

                newPairsArrey[i] = pairs[l_n_b];
                l_n_b += 1;

            }
            /*
                If the number in the left side is equal to the number on the right side,
                write any of the in the array fir alerady sorted numbers.
                In this case the number in the left sie is written.
            */
            else if (strongOfLeftPair == strongOfRightPair)
            {
                newPairsArrey[i] = pairs[l_n_b];
                l_n_b +=1;
            }
        }
        for (int i = 0; i < n; i++)
        {
            pairs[b2 + i] = newPairsArrey[i];
        }

        // for (int i = 0; i < pair_count; i++)
        // {
        //     printf("pair: %d\nwin: %d\nlos: %d\n\n", i, pairs[i].winner, pairs[i].loser);
        // }

    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        //color[] - array for dfsForLoop(), in which the status of the graph verices is denoted
        int color[candidate_count];
        if (!findLoop(pairs[i].winner))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
        //Reset the status of the vertices in the color[] arrey.
        for (int j = 0; j < candidate_count; j++)
        {
            color[j] = 0;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // The index of the winning candidate in the candidates[] arrey.
    int winner;
    degCandidate candidatesDeg[candidate_count];

    //Fill struct candidatesDeg.
    for (int i = 0; i < candidate_count; i++)
    {
        //Assign the candidate indeces.
        candidatesDeg[i].number = i;
        //Fill outgoingEdges of all candidates with 0.
        candidatesDeg[i].outgoingEdges = 0;
    }
    // See how many outgoing edhes each candidate vertex has.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if(locked[candidatesDeg[i].number][j] == true)
            {
                candidatesDeg[i].outgoingEdges += 1;
            }
        }
    }

    sort_candidatesDeg(0, candidate_count - 1, candidatesDeg);

    //Check if there is no more than one winner
    int num_of_winners = 1;
    for (int i = 0; i < candidate_count; i++)
    {
        if(candidatesDeg[i].outgoingEdges == candidatesDeg[i + 1].outgoingEdges)
        {
            num_of_winners += 1;
        }
        else
        {
            break;
        }
    }

    //Look at the number of points of all candidates.
    for(int i = 0; i < candidate_count; i++)
    {
        printf("%i)source %i: %i\n", i, candidatesDeg[i].number, candidatesDeg[i].outgoingEdges);
    }

    //Print winner(s).
    for(int i = 0; i < num_of_winners; i++)
    {
        printf("%s " , candidates[candidatesDeg[i].number]);
    }
    printf("\n");
}
    //Looking for whether there is a cycle in the graph locked[][].
    bool findLoop(int vertex)
    {
        //color - status of vertex.
        int color[candidate_count];
        int num_of_vertex = sizeof(color) / sizeof(color[0]);
        //loop - shows if we have found a cycle in the graph
        bool loop = false;

        //If we haven't already visited the vertex[i]: Run DFS.
        for (int i = 0; i < num_of_vertex; i++)
        {
            if (color[i] == 0)
            {
                dfsForLoop(vertex, color, &loop, locked);
            }
        }

        return loop;
    }
/*
    dfsForLoop(for locked[][]) - function to search for a lop in the graph, using Depth First Search(DFS).
    V - current vertex.
    color[i] = 0 - white - this is the vertex to which we have not yet paved the way.
    color[i] = 1 - grey - this is the vertex of the path on which we now follow(we find the cicle).
    color[i] = 2 - black - this is the vertex belonging to one of the passed paths.
*/
void dfsForLoop(int current_vertex,int color[], bool *loop,bool adjacency_matrix[][MAX])
{
    //color[V] = grey;
    color[current_vertex] = 1;
    int num_of_vertex = sizeof(*color) / sizeof(color[0]);

    //Looking for vertices connected with V by edges.
    for (int i = 0; i < num_of_vertex; i++)
    {
        /*
            If a vertex associated with the V is found,
            and it has not yet been part of the path:
            Move to it.
        */
        if (adjacency_matrix[current_vertex][i] == true)
        {
            if (color[i] == 0)
            {
                dfsForLoop(i, color, loop, adjacency_matrix);
            }
            // We found the cycle.
            else if (color[i] == 1)
            {
                *color = true;
            }
        }
    }
    // color[V] - black.
    color[current_vertex] = 2;
}

// Sort pairs in decreasing order by strength of victory
void sort_candidatesDeg(int arr_begin, int arr_end, degCandidate candidatesDeg[])
{
    // int arr_begin = 0;
    // int arr_end = sizeof(pairs) / sizeof(pairs[0]);
    //Turn the array so that its beginning is 1.
    //b - begin of array
    int b = arr_begin + 1;
    //e - end of array
    int e = arr_end + 1;
    int b2 = b - 1;


    if (b == e)
    //e - b + 1 == 1
    {
    }
    else if (b < e)
    {
        /*
            n - numer of array elements;
            n_n - next number(half of the current array);
            r - reminder;

            l_n_b - left next begin(the beginning of the left part of the array);
            l_n_e - left next end(the ending of the left part of the array);

            r_n_b - right next begin(the beginning of the right part of the array);
            r_n_e - right next end(the ending of the right part of the array);

        */
        int n, n_n, r, l_n_e, l_n_b, r_n_b, r_n_e;

        n = e - b + 1;
        n_n = n / 2;
        r = n % 2;

        l_n_e = b + n_n + r - 1 - 1;
        l_n_b = b - 1;

        r_n_b = e - n_n + 1 - 1;
        r_n_e = e - 1;

        //Sorting the left half of the array.
        sort_candidatesDeg(l_n_b, l_n_e, candidatesDeg);
        //Sorting the right half of the array.
        sort_candidatesDeg(r_n_b, r_n_e, candidatesDeg);

        degCandidate newCandidateArrey[n];

        //Marge sorted halves of the array.
        for (int i = 0; i < n; i++)
        {

            if(r_n_b > r_n_e)
            {
                newCandidateArrey[i] = candidatesDeg[l_n_b];
                //Go to the next number in the right part.
                l_n_b += 1;
                continue;
            }
            else if(l_n_b > l_n_e)
            {
                newCandidateArrey[i] = candidatesDeg[r_n_b];
                //Go to the next number in the left part.
                r_n_b +=1;
                continue;
            }

            int strongOfLeftPair = (candidatesDeg[l_n_b].outgoingEdges);
            int strongOfRightPair = (candidatesDeg[r_n_b].outgoingEdges);
            if (strongOfLeftPair < strongOfRightPair)
            {
                /*
                    If the number in the left half is smaller then the number in the right half.
                    And the left half is not passed(begin in not biger than the end of left half).
                    Write the left number in the array to for already sorted numbers.
                */

                //If the left half is already passed. Wirte the remaining number in the right half.

                newCandidateArrey[i] = candidatesDeg[r_n_b];
                r_n_b +=1;

            }
            else if (strongOfLeftPair > strongOfRightPair)
            {
                /*
                    If the number in the right half is smaller then the number in the left half.
                    And the right half is not passed(begin in not biger than the end of right half).
                    Write the left number in the array to for already sorted numbers.
                */

                //If the right half is already passed. Wirte the remaining number in the left half.
                    newCandidateArrey[i] = candidatesDeg[l_n_b];
                    l_n_b += 1;

            }
            /*
                If the number in the left side is equal to the number on the right side,
                write any of the in the array fir alerady sorted numbers.
                In this case the number in the left sie is written.
            */
            else if (strongOfLeftPair == strongOfRightPair)
            {
                newCandidateArrey[i] = candidatesDeg[l_n_b];
                l_n_b +=1;
            }
        }
        for (int i = 0; i < n; i++)
        {
            candidatesDeg[b2 + i] = newCandidateArrey[i];
        }

        // for (int i = 0; i < pair_count; i++)
        // {
        //     printf("pair: %d\nwin: %d\nlos: %d\n\n", i, pairs[i].winner, pairs[i].loser);
        // }

    }
}



