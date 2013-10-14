/*First attempt at an AI, using the following rules:
  1. If the value of the pile is 0, 5, 10, 15, 20 etc. play a penny.
  2. If the value of the pile is 1-4, play a nickel.
  3. If the value of the pile is 6-9, play a dime.
  4. If the value of the pile is 21-24, 26-29, 31-34, 36-39 etc. play a quarter.*/
#include "cppFIGHT.h"

namespace Elle{

   using namespace CPPFight;

   class EllePlayer : public Player
      {
         public:
           EllePlayer() : Player( "CatBites", "Elle" )
              {
               }
            virtual Move GetMove( const Game& theGame )
             {
                const Change& myChange   = theGame.GetPlayerChange(theGame.GetCurrentPlayer());
              const Change& thePile = theGame.GetGameChange();

                  //check first rule, if no pennies in the pile, play a penny.
                  if (!(thePile.GetCount(CPPFight::PENNY))) {
                     return CPPFight::PENNY;
                     }

                  //check second rule, if there are only pennies in the pile, play nickel.
                  else if ((thePile.GetCount(CPPFight::PENNY)) > 0 && (!(thePile.GetCount(CPPFight::NICKEL))) && (!(thePile.GetCount(CPPFight::DIME))) && (!(thePile.GetCount(CPPFight::QUARTER)))) {

                     if ((myChange.GetCount(CPPFight::NICKEL)))
                        return Move (CPPFight::NICKEL, MaximumChangeValue(thePile, CPPFight::NICKEL));

                     else if ((myChange.GetCount(CPPFight::DIME)))
                        return Move (CPPFight::DIME, MaximumChangeValue(thePile, CPPFight::DIME));

                     else if ((myChange.GetCount(CPPFight::QUARTER)))
                        return Move (CPPFight::QUARTER, MaximumChangeValue(thePile, CPPFight::QUARTER));

                     else {
                        return CPPFight::PENNY;
                     }
                  }

                  //check third rule, if there are only pennies and one nickel in the pile, play dime.
                  else if ((thePile.GetCount(CPPFight::PENNY)) > 0 && ((thePile.GetCount(CPPFight::NICKEL))==1) && (!(thePile.GetCount(CPPFight::DIME))) && (!(thePile.GetCount(CPPFight::QUARTER)))) {

                     if ((myChange.GetCount(CPPFight::DIME)))
                        return Move (CPPFight::DIME, MaximumChangeValue(thePile, CPPFight::DIME));

                     else if ((myChange.GetCount(CPPFight::NICKEL)))
                        return Move (CPPFight::NICKEL, MaximumChangeValue(thePile, CPPFight::NICKEL));

                     else if ((myChange.GetCount(CPPFight::QUARTER)))
                        return Move (CPPFight::QUARTER, MaximumChangeValue(thePile, CPPFight::QUARTER));

                     else {
                        return CPPFight::PENNY;
                     }
                  }

                  //check fourth rule, if there are pennies and more than a dime in the pile, play quarter.
                  else {

                     if ((myChange.GetCount(CPPFight::QUARTER)))
                        return Move (CPPFight::QUARTER, MaximumChangeValue(thePile, CPPFight::QUARTER));

                     else if ((myChange.GetCount(CPPFight::NICKEL)))
                        return Move (CPPFight::NICKEL, MaximumChangeValue(thePile, CPPFight::NICKEL));

                     else if ((myChange.GetCount(CPPFight::DIME)))
                        return Move (CPPFight::DIME, MaximumChangeValue(thePile, CPPFight::DIME));

                     else {
                        return CPPFight::PENNY;
                     }
                  };
             }
      }PlayerElle;
}

/*My attempt at an AI, using the following rules:
  1. If the value of the pile is 0, 5, 10, 15, 20 etc. play a penny.
  2. If the value of the pile is 1-4, play a nickel.
  3. If the value of the pile is 6-9, play a dime.
  4. If the value of the pile is 21-24, 26-29, 31-34, 36-39 etc. play a quarter.
  5. Maximise the number of coins taken in change without making a loss in value of coins. */

namespace Elle2{

   using namespace CPPFight;

   class EllePlayer : public Player
      {
         public:
           EllePlayer() : Player( "CatBites2", "Elle" )
              {
               }
            virtual Move GetMove( const Game& theGame )
             {
                const Change& myChange   = theGame.GetPlayerChange(theGame.GetCurrentPlayer());
                const Change& thePile = theGame.GetGameChange();
                Coin myCoin;
                int bestLoss, bestCount = 0;
                std::vector <Change> theChange;
                Change bestChange;

                  //check first rule, if no pennies in the pile, play a penny.
                  if (!(thePile.GetCount(CPPFight::PENNY)))
                     return CPPFight::PENNY;

                  //check second rule, if there are only pennies in the pile, play nickel.
                  else if ((thePile.GetCount(CPPFight::PENNY)) > 0 && (!(thePile.GetCount(CPPFight::NICKEL))) && (!(thePile.GetCount(CPPFight::DIME))) && (!(thePile.GetCount(CPPFight::QUARTER)))) {

                     if ((myChange.GetCount(CPPFight::NICKEL)))
                        myCoin = CPPFight::NICKEL;

                     else if ((myChange.GetCount(CPPFight::DIME)))
                        myCoin = CPPFight::DIME;

                     else if ((myChange.GetCount(CPPFight::QUARTER)))
                        myCoin = CPPFight::QUARTER;

                     else {
                        return CPPFight::PENNY;
                     }
                  }

                  //check third rule, if there are only pennies and one nickel in the pile, play dime.
                  else if ((thePile.GetCount(CPPFight::PENNY)) > 0 && ((thePile.GetCount(CPPFight::NICKEL))==1) && (!(thePile.GetCount(CPPFight::DIME))) && (!(thePile.GetCount(CPPFight::QUARTER)))) {

                     if ((myChange.GetCount(CPPFight::DIME)))
                        myCoin = CPPFight::DIME;

                     else if ((myChange.GetCount(CPPFight::NICKEL)))
                        myCoin = CPPFight::NICKEL;

                     else if ((myChange.GetCount(CPPFight::QUARTER)))
                        myCoin = CPPFight::QUARTER;

                     else {
                        return CPPFight::PENNY;
                     }
                  }

                  //check fourth rule, if there are pennies and more than a dime in the pile, play quarter.
                  else {

                     if ((myChange.GetCount(CPPFight::QUARTER)))
                        myCoin = CPPFight::QUARTER;

                     else if ((myChange.GetCount(CPPFight::NICKEL)))
                        myCoin = CPPFight::NICKEL;

                     else if ((myChange.GetCount(CPPFight::DIME)))
                        myCoin = CPPFight::DIME;

                     else {
                        return CPPFight::PENNY;
                     }
                  };
               //check fifth rule, find the change option with the most coins and minimum loss.
               GetAllPossibleChange(thePile, myCoin, theChange);
                  for(std::vector <Change>::iterator i = theChange.begin(); i != theChange.end(); i++){
                 const Change testChange = *i;
                const int loss  = myCoin - testChange.GetTotalValue();
                const int count = testChange.GetTotalCount();

                if ((count > bestCount) || (count == bestCount && (loss < bestLoss)))
                   {
                     bestLoss   = loss;
                           bestCount  = count;
                           bestChange = testChange;
                   }
                     };
                     return Move (myCoin, bestChange);
             }
      }PlayerElle2;
}
