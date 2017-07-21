struct card{

    int rank;
    char suite[SUIT];


} card;

enum suites{ 
SPADES, DIAMONDS, CLUBS, HEARTS};

/* ***************************Prototypes 
     ************************/

void deck_fill( struct card blank_deck[] );
void shuffle( struct card deck[] );





int main( void )
{
    struct card deck[DECK], original_deck[DECK];
    int i = 1, k = 0;

    deck_fill( deck);
    deck_fill(original_deck);

    shuffle(deck);

    deck_fill(deck);




    do{
        k += 1;
        shuffle(deck);



        if ( (! ( strcmp( original_deck[1].suite, deck[1].suite))) 
                    && ( original_deck[3].rank == deck[3].rank))
        {

            break;

        }


    } while( i );


    printf( "It took %d shuffles to get back to the original deck.", 
                                                          k );

    return 0;
}





/* Definitions */
void deck_fill( struct card blank_deck[] )
{
    /** function to "make" the deck***/

    struct card *deck_change = blank_deck;
    int i, k = SPADES, j;



    while (k < 4)
    {

        for (i = 0; i < 13; i++)
        {
            switch(k)
            {
                case (SPADES): 
                    memcpy(deck_change[i].suite, 
                                                                        "SPADES", 7);
                    deck_change[i].rank = i + 1;
                    break;

                case (DIAMONDS):            
                    memcpy(deck_change[i+13].suite, 
                                                 "DIAMONDS", 9);
                    deck_change[i+13].rank = i + 1;
                    break;

                case (CLUBS):
                    memcpy(deck_change[i + 26].suite, 
                                              "CLUBS", 6);
                    deck_change[i+26].rank = i + 1;
                    break;

                case (HEARTS): 
                    memcpy(deck_change[i + 39].suite, 
                                                                            "HEARTS", 7);
                    deck_change[i+39].rank = i + 1;
                    break;
            }



        }

        k++;

    }

}

void shuffle( struct card deck[] )
{

    /**function to shuffle the deck*/

    struct card first_half[DECK], sec_half[DECK];
    struct card *deck_shuffle = deck;
    int ct;

    for (ct = 0; ct < 26; ct++)
    {
        first_half[ct].rank = deck[ct].rank;
        strcpy(first_half[ct].suite, deck[ct].suite);
        sec_half[ct].rank = deck[ct+26].rank;
        strcpy(sec_half[ct].suite, deck[ct+26].suite);
    }

    for (ct = 0; ct < 26; ct++)
    {

        deck_shuffle[ct * 2].rank = first_half[ct].rank;
        strcpy(deck_shuffle[ct * 2].suite, first_half[ct].suite);
        deck_shuffle[(ct*2)+1].rank = sec_half[ct].rank;
        strcpy(deck_shuffle[(ct*2)+1].suite, 
                                                               sec_half[ct].suite);

    }

}