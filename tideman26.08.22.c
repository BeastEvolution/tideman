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
    // Candidate number.
    int number;
    // The number of ribs that depart from the vertex of the candidate.
    int incomingEdges;
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
void sort_pairs(void);
void margeSortForPairs(int arr_begin,int arr_end);
void lock_pairs(void);
void print_winner(void);
bool findLoop(void);
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
    // Write down the names of the candodates in candidates[].
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

    // Open file with input data(if need to get input form file).
    // FILE *file = fopen("inpute.txt", "r");

    int voter_count = get_int("Number of voters: ");

    // Get "number of voters"(voter_count) form file;
    // fscanf(file, "%i ", &voter_count);

    // Get the number of voter using scanf().
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
            string name  = get_string("Rank %i: ", j + 1);
            // If need to set the size of the string name.
            // = malloc(10);

            //  Get the string name value from the file.
            //fscanf(file, "%s ", name);

            // Get the candidate name using scanf().
            // char *name = malloc(2);
            // printf("Rank %i: ", j + 1);
            // scanf("%s", name);
            // printf("\n");

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }

            //Free up memory if we using the malloc();
            //free(name);
        }

        record_preferences(ranks);

        printf("\n");

        // for(int j = 0; j < candidate_count; j++)
        // {
        //     printf("%d\n", ranks[j]);
        // }
    }

    //Close file with input data.
    //fclose(file);

    add_pairs();

    // OutPut: Shows the matrix preferences[i][j]
    // for (int i = 0; i < candidate_count + 2; i++)
    // {
    //     if (i == 0)
    //     {
    //         for (int j = 0; j < candidate_count + 2; j++)
    //         {
    //             if (j < 2)
    //             {
    //                 printf(" ");
    //             }
    //             else
    //             {
    //                 printf("%i ", (j - 2));
    //             }
    //         }
    //     }
    //     else if (i == 1)
    //     {
    //         for (int j = 0; j < candidate_count + 2; j++)
    //         {
    //             if(j < 2)
    //             {
    //                 printf(" ");
    //             }
    //             else
    //             {
    //                 printf("- ");
    //             }
    //         }

    //     }
    //     else
    //     {
    //         for (int j = 0; j < candidate_count + 1; j++)
    //         {
    //             if(j == 0)
    //             {
    //                 printf("%i|", (i - 2));
    //             }
    //             else
    //             {
    //                 printf("%d ", preferences[i - 2][j - 1]);
    //             }
    //         }
    //     }
    //         printf("\n");
    // }
    // printf("\n");

    // OutPut: Shows the winner and loser of the struct pair(pairs)
    // for (int i = 0; i < pair_count; i++)
    //     {
    //         printf("pair: %d\nwin: %d\nlos: %d\n\n", i, pairs[i].winner, pairs[i].loser);
    //     }

    /*
        OutPut: Shows the strength of each pair,
        in the difference of votes between the winner and the loser.
    */
    // for(int i = 0; i < pair_count; i++)
    // {
    //     printf("pair strength %i: %i\n", i, (preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner]));
    // }
    // printf("\n");

    sort_pairs();

    lock_pairs();

    // OutPut: Shows the winner and loser of the struct pair(pairs)
    // for (int i = 0; i < pair_count; i++)
    //     {
    //         printf("pair: %d\nwin: %d\nlos: %d\n\n", i, pairs[i].winner, pairs[i].loser);
    //     }

    /*
        OutPut: Shows the strength of each pair,
        in the difference of votes between the winner and the loser.
    */
    // for(int i = 0; i < pair_count; i++)
    // {
    //     printf("pair strength %i: %i\n", i, (preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner]));
    // }
    // printf("\n");

     // OutPut: Shows the matrix locked
    // for (int i = 0; i < candidate_count; i++)
    // {
    //     for (int j = 0; j < candidate_count; j++)
    //     {
    //         if(locked[i][j])
    //         {
    //             printf("T ");
    //         }
    //         else
    //         {
    //             printf("F ");
    //         }
    //     }
    //     printf("\n");
    // }

    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Candidate index in arrey candidates[]
    int candidate_index;

    // Check whether the candidate's name is in the candidates[].
    for (int i = 0; i < candidate_count; i++)
    {
        if(!strcmp(name, candidates[i]))
        {
            candidate_index = i;
            break;
        }

        // If the name of the candidate is not found in array of c. even in last iteration of the cycle, the function will return false.
        if(i == candidate_count - 1)
        {
            return false;
        }
    }

    // If the candidate is already in vote, return false.
    for (int i = 0; i < rank; i++)
    {
        if(candidate_index == ranks[i])
        {
            return false;
        }
    }

    ranks[rank] = candidate_index;
    return true;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // Compare pairs in Vote, from top to bottom(ranks[0](first candidate in Vote) and ranks[1](second candidate in Vote),[1] and [2]...).
    // And record a +1 in preferences[][] ([0][1],[1][2],...), to the winner of that pair.
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
    // Compare all candidates in pairs.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // Compare the number of votes for one or another candidate in pairs
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                // Increase the number of pairs.
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

void sort_pairs(void)
{
    int arr_begin = 0;
    int arr_end = sizeof(pairs) / sizeof(pairs[0]);
    margeSortForPairs(arr_begin, arr_end);
}

// Sort pairs in decreasing order by strength of victory (Z-A).
void margeSortForPairs(int arr_begin,int arr_end)
{

    // Turn the array so that its beginning is 1.
    // b - begin of array
    int b = arr_begin + 1;
    // e - end of array
    int e = arr_end + 1;
    // int b2 = b - 1;

    // If there is no left and right part. Going up a lefel above.
    if (b == e)
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

        // Find the left part of the array.
        l_n_e = b + n_n + r - 1 - 1;
        l_n_b = b - 1;

        // Find the right part of the array.
        r_n_b = e - n_n + 1 - 1;
        r_n_e = e - 1;

        // Sorting the left half of the array.
        margeSortForPairs(l_n_b, l_n_e);
        // Sorting the right half of the array.
        margeSortForPairs(r_n_b, r_n_e);

        // Create a temporary array to store the sorted elementes of array.
        pair newPairsArrey[n];

        // Marge sorted halves of the array.
        // Compare the values of the begininngs of the lfet and right halves (l_n_b and r_n_b) of the array.
        for (int i = 0; i < n; i++)
        {
            // If the right half is already passed. Wirte the remaining number in the left half.
            if(r_n_b > r_n_e)
            {
                newPairsArrey[i] = pairs[l_n_b];
                 // Go to the next number in the left part.
                l_n_b += 1;
                continue;
            }
            else if(l_n_b > l_n_e)
            {
                newPairsArrey[i] = pairs[r_n_b];
                // Go to the next number in the right part.
                r_n_b +=1;
                continue;
            }

            // Write the dufferece in the votes of the winner over the loser into temporary variables.
            int strongOfLeftPair = (preferences[pairs[l_n_b].winner][pairs[l_n_b].loser] - preferences[pairs[l_n_b].loser][pairs[l_n_b].winner]);
            int strongOfRightPair = (preferences[pairs[r_n_b].winner][pairs[r_n_b].loser] - preferences[pairs[r_n_b].loser][pairs[r_n_b].winner]);

            /*
                If the number in the left half is smaller then the number in the right half.
                And the left half is not passed(begin in not biger than the end of left half).
                Write the right(depends on the sorting issue A-Z or Z-A) number in the array to for already sorted numbers.
            */
            if (strongOfLeftPair < strongOfRightPair)
            {
                newPairsArrey[i] = pairs[r_n_b];
                r_n_b +=1;
            }
            /*
                If the number in the right half is smaller then the number in the left half.
                And the right half is not passed(begin in not biger than the end of right half).
                Write the left(depends on the sorting issue A-Z or Z-A) number in the array to for already sorted numbers.
            */
            else if (strongOfLeftPair > strongOfRightPair)
            {
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

        // Write the sorted part into our array.
        for (int i = arr_begin, j = 0; i <= arr_end; i++, j++)
        {
            pairs[i] = newPairsArrey[j];
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
    // Write the edges of pairs in form pairs[] to locked[].
    // And ignore an edge in case it creates a loop in graph.
    for (int i = 0; i < pair_count; i++)
    {
        // color[] - array for dfsForLoop(), in which the status of the graph verices is denoted
        // int color[candidate_count];

        locked[pairs[i].winner][pairs[i].loser] = true;
        // If the current edge creates a loop in the matrix: Delete it.
        if (findLoop())
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
        //Reset the status of the vertices in the color[] arrey.
        // for (int j = 0; j < candidate_count; j++)
        // {
        //     color[j] = 0;
        // }
    }
}

// Print the winner of the election
void print_winner(void)
{
    // The index of the winning candidate in the candidates[] arrey.
    int winner;
    degCandidate candidatesDeg[candidate_count];

    // Fill struct candidatesDeg.
    for (int i = 0; i < candidate_count; i++)
    {
        // Assign the candidate indeces, to find it in string candidates[].
        candidatesDeg[i].number = i;
        // Fill incomingEdges of all candidates with 0.
        candidatesDeg[i].incomingEdges = 0;
    }
    // See how many outgoing edhes each candidate vertex has.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            // Looking at the edges of candidate i to the other candidates.
            if(locked[j][candidatesDeg[i].number] == true)
            {
                candidatesDeg[i].incomingEdges += 1;
            }
        }
    }

     sort_candidatesDeg(0, candidate_count - 1, candidatesDeg);

    // Check if there is no more than one winner
    int num_of_winners = 1;
    for (int i = 0; i < candidate_count; i++)
    {
        if(candidatesDeg[i].incomingEdges == candidatesDeg[i + 1].incomingEdges)
        {
            num_of_winners += 1;
        }
        else
        {
            break;
        }
    }

    //Look at the number of points of all candidates.
    // for(int i = 0; i < candidate_count; i++)
    // {
    //     printf("%i)source %i: %i\n", i, candidatesDeg[i].number, candidatesDeg[i].incomingEdges);
    // }

    // Print winner(s).
    for(int i = 0; i < num_of_winners; i++)
    {
        printf("%s " , candidates[candidatesDeg[i].number]);
    }
    printf("\n");

}

    // OutPut: Looking for whether there is a cycle in the graph locked[][].
    bool findLoop(void)
    {
        // color - status of vertex.

        int color[candidate_count];
        // int current_vertex = sizeof(color) / sizeof(color[0]);

        //loop - shows if we have found a cycle in the graph
        bool loop = false;

        // Fill color[] with 0.
        for(int i = 0; i < candidate_count; i++)
        {
            color[i] = 0;
        }

        //If we haven't already visited the vertex[i]: Run DFS on this vertex i.
        for (int i = 0; i < candidate_count; i++)
        {
            if (loop)
            {
                break;
            }
            else if (color[i] == 0)
            {
                dfsForLoop(i, color, &loop, locked);
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
    // color[V] = grey;
    color[current_vertex] = 1;

    // Looking for vertices connected with V by edges.
    for (int i = 0; i < candidate_count; i++)
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
                *loop = true;
            }
        }
    }
    // color[V] - black.
    color[current_vertex] = 2;
}

/*
    Sort pairs in decreasing order by strength of victory (A-Z).
    Arguments:
    1 - arr_begin - first index of array
    2 - arr_end - last index of array
    3 - candidatesDeg[] - array to be sorted
*/
void sort_candidatesDeg(int arr_begin, int arr_end, degCandidate candidatesDeg[])
{
    // Turn the array so that its beginning is 1.
    // b - begin of array
    int b = arr_begin + 1;
    // e - end of array
    int e = arr_end + 1;
    // int b2 = b - 1;

    // If there is no left and right part. Going up a lefel above.
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

        // Find the left part of the array.
        l_n_e = b + n_n + r - 1 - 1;
        l_n_b = b - 1;

        // Find the right part of the array.
        r_n_b = e - n_n + 1 - 1;
        r_n_e = e - 1;

        // Sorting the left half of the array.
        sort_candidatesDeg(l_n_b, l_n_e, candidatesDeg);
        // Sorting the right half of the array.
        sort_candidatesDeg(r_n_b, r_n_e, candidatesDeg);

        // Create a temporary array to store the sorted elementes of array.
        degCandidate newCandidateArrey[n];

        // Marge sorted halves of the array.
        // Compare the values of the begininngs of the lfet and right halves (l_n_b and r_n_b) of the array.
        for (int i = 0; i < n; i++)
        {

            // If the left half is already passed. Wirte the remaining number in the right half.
            if(l_n_b > l_n_e)
            {
                newCandidateArrey[i] = candidatesDeg[r_n_b];
                // Go to the next number in the right part.
                r_n_b += 1;
                continue;
            }
            // If the right half is already passed. Wirte the remaining number in the left half.
            else if(r_n_b > r_n_e)
            {
                newCandidateArrey[i] = candidatesDeg[l_n_b];
                // Go to the next number in the left part.
                l_n_b +=1;
                continue;
            }

            // Write the dufferece in the votes of the winner over the loser into temporary variables.
            int strongOfLeftPair = (candidatesDeg[l_n_b].incomingEdges);
            int strongOfRightPair = (candidatesDeg[r_n_b].incomingEdges);

            /*
                If the number in the left half is smaller then the number in the right half.
                Write the left(depends on the sorting issue A-Z or Z-A) number in the array for already sorted numbers.
            */
            if (strongOfLeftPair < strongOfRightPair)
            {
                newCandidateArrey[i] = candidatesDeg[l_n_b];
                l_n_b +=1;
            }
            /*
                If the number in the right half is smaller then the number in the left half.
                Write the right(depends on the sorting issue A-Z or Z-A) number in the array for already sorted numbers.
            */
            else if (strongOfLeftPair > strongOfRightPair)
            {
                    newCandidateArrey[i] = candidatesDeg[r_n_b];
                    r_n_b += 1;
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
        // Write the sorted part into our array.
        for (int i = arr_begin, j = 0; i <= arr_end; i++, j++)
        {
            candidatesDeg[i] = newCandidateArrey[j];
        }

        // for (int i = 0; i < pair_count; i++)
        // {
        //     printf("pair: %d\nwin: %d\nlos: %d\n\n", i, pairs[i].winner, pairs[i].loser);
        // }

    }
}



