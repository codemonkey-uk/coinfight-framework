var FIGHT=require('./jsFIGHT.js');

function CDumbAI()
{
	this.name = "DumbAI";
	this.pickCoin = function(game, player) {
		var result;
		for (var i = 0; i < player.coins.coins.length; ++i){		
			if (player.coins.coins[i] > 0) {
				result = i;
				break;
			}
		}
		return result;
	}
	this.takeChange = function(game, player) {
		var result = -1;
		for (var i = 0; i < game.change.coins.length; ++i){		
			if (game.canTakeChange(i) ) {
				result = i;
				break;
			}
		}
		return result;
	}
}

function MaximumChangeValue(availableChange, coinIndex)
{
	var result = new FIGHT.CChange(FIGHT.EMPTY_CHANGE);
	var coinValue = FIGHT.COINLIST[coinIndex];

	// work down through coin types
	for (var testIndex = coinIndex - 1; testIndex >= 0; --testIndex)
	{
		var testCoin = FIGHT.COINLIST[testIndex];

		var maxValue = coinValue - result.getTotalValue() - 1;
		var maxCoins = Math.floor(maxValue / testCoin);
			
		var numCoinsToTake = 
			Math.min(maxCoins, availableChange.getCount(testIndex));

		if (numCoinsToTake > 0)
		{
			result.insertCoins(testIndex, numCoinsToTake);
		}
	}

	return result;
}

function MaximumChangeCount(availableChange, coinIndex)
{
	var result = new FIGHT.CChange(FIGHT.EMPTY_CHANGE);
	var coinValue = FIGHT.COINLIST[coinIndex];

	// work up through coin types
	for (var testIndex = 0; testIndex < coinIndex; testIndex++)
	{
		var testCoin = FIGHT.COINLIST[testIndex];

		var maxValue = coinValue - result.getTotalValue() - 1;
		var maxCoins = Math.floor(maxValue / testCoin);
			
		var numCoinsToTake = 
			Math.min(maxCoins, availableChange.getCount(testIndex));

		if (numCoinsToTake > 0)
		{
			result.insertCoins(testIndex, numCoinsToTake);
		}
	}

	return result;
}


function CMinLossGreedyAI()
{
	this.name = "Shipman Thaddock";
	this.pickCoin = function(game, player) {
		
		var bestCoin;
		var bestChange;
		
		var bestLoss  = -1;
		var bestCount = 0;
			
		for (var i = 0; i < player.coins.coins.length; ++i){		
			if (player.coins.coins[i] > 0) 
			{
				var testChange = MaximumChangeValue(game.change, i);
				var loss = FIGHT.COINLIST[i] - testChange.getTotalValue();
				var count = testChange.getTotalCount();
				if (((loss < bestLoss) || (bestLoss < 0)) ||
					((loss == bestLoss) && (count > bestCount)))
				{
					//remember this move
					bestLoss   = loss;
					bestCount  = count;
					bestCoin   = i;
					bestChange = testChange;
				}				
			}
		}
		
		this.changeToTake = bestChange;
		
		return bestCoin;
	}
	this.takeChange = function(game, player) {
		var result = -1;
		for (var i = 0; i < this.changeToTake.coins.length; ++i){		
			if (this.changeToTake.coins[i]>0) {
				result = i;
				this.changeToTake.coins[i]--;
				break;
			}
		}
		return result;
	}
} // end of class: CMinLossGreedyAI

function CWookieAI()
{
	this.name = "Ensign Thaddock";
	this.pickCoin = function(game, player) {
	
		// If the number of coins that will be won is greater than 2*CoinValue / 5
		// or the value of the coins adds up to greater than 2*CoinValue / 3
		for (var i = player.coins.coins.length-1; i > 0 ; --i)
		{		
			if (player.coins.coins[i] > 0) 
			{
				var test2Coin = 2*FIGHT.COINLIST[i];
				var coinNumMin = Math.floor(test2Coin / 5);
				var coinValMin = Math.floor(test2Coin / 3);
					
				var testChange = MaximumChangeValue(game.change, i);
				
				if ((testChange.getTotalCount() > coinNumMin) ||
					(testChange.getTotalValue() > coinValMin))
				{
					this.changeToTake = testChange;
					return i;
				}
			}
		}
		
		// if there are 5's in hand, and pennies in the pot, then better to 
		// take the pennies than throw out.
		if ((player.coins.coins[1]>0) && (game.change.coins[0]>0)) 
		{	
			this.changeToTake = MaximumChangeValue(game.change, 1);
			return 1;
		}
		
		// Try to throw out a penny
		if (player.coins.coins[0]>0) 
		{
			this.changeToTake = null;
			return 0;
		}
			
		// Get the most coins
		var bestCoin;
		var grabCount = -1;
		for (var i = 0; i < player.coins.coins.length; ++i)
		{			
			if (player.coins.coins[i] > 0) 
			{
				var tempChangeCount = MaximumChangeValue(game.change, i).getTotalCount();
				if (tempChangeCount > grabCount)
				{
					bestCoin = i;
					grabCount = tempChangeCount;
				}
			}
		}
		
		this.changeToTake = MaximumChangeValue(game.change, bestCoin);
		return bestCoin;
	}
	
	this.takeChange = function(game, player) {
		var result = -1;
		if (this.changeToTake)
		{
			for (var i = 0; i < this.changeToTake.coins.length; ++i){		
				if (this.changeToTake.coins[i]>0) {
					result = i;
					this.changeToTake.coins[i]--;
					break;
				}
			}
		}
		return result;
	}
} // end of class: CWookieAI

function CEvalAI(depth)
{
	this.name = "Captain Thaddock";
	this.values = [ 1, 2, 4, 9 ];
	this.changeStratigies = [ MaximumChangeValue, MaximumChangeCount ];
	
	if (typeof depth == "number")
		this.depth = depth;
	else this.depth = 6;
		
	this.evaluate = function(playerChange, otherPlayerChange)
	{
		var result = 0;
		
		for (var i = 0; i < playerChange.coins.length; ++i)
			result += playerChange.coins[i]*this.values[i]; 
		for (var i = 0; i < otherPlayerChange.coins.length; ++i)
			result -= otherPlayerChange.coins[i]*this.values[i]; 
			
		return result;
	}
	
	
	this.pickCoin = function(game, player) {
		// this.debugString = "";
		this.result = this.pickMove(
			game.change.copy(), 
			player.coins.copy(), 
			game.getOtherPlayer( player ).coins.copy(),
			this.depth + (4 - player.coins.getCoinTypeCount()),
			-Number.MAX_VALUE,
			Number.MAX_VALUE
		);
		// _debug.innerHTML = this.debugString;
		
		return this.result.coin;
	}
	
	this.pickMove = function(tableChange, playerChange, otherPlayerChange, d, alpha, beta) {
		
		var result = new CMove(-1);
		result.bestScore = -Number.MAX_VALUE;
		
		for (var i = 0; i < playerChange.coins.length; ++i){		
			if (playerChange.coins[i] > 0) 
			{
				// for each coin taking stratigy
				for (var s = 0; s < this.changeStratigies.length; ++s)
				{
					var testChange = this.changeStratigies[s](tableChange, i);
					
					// apply the move to the game-state objects:					
					playerChange.subtract( i );
					tableChange.add( i );					
					tableChange.subtract( testChange );
					playerChange.add( testChange );
					
					// score the modified game state
					var score;
					if (d===0 || otherPlayerChange.empty())
					{
						score = this.evaluate( playerChange, otherPlayerChange );						
					}
					else
					{
						var m = this.pickMove(tableChange, otherPlayerChange, playerChange, d-1, -beta, -alpha);
						score = -m.bestScore; 
					}
					
					// roll back the move ready to test the next posible move:
					playerChange.add( i );
					tableChange.subtract( i );					
					tableChange.add( testChange );
					playerChange.subtract( testChange );

					// compare score to best so far
					if (score > result.bestScore)
					{
						//remember this move
						result.bestScore = alpha = score;
						result.coin = i;
						result.change = testChange;
						
						// prune:
						if (beta<=alpha)
						{
							return result;
						}
					}
				}
			}
		}
		return result;
	}
	this.takeChange = function(game, player) {
		var result = -1;
		for (var i = 0; i < this.result.change.coins.length; ++i){		
			if (this.result.change.coins[i]>0) {
				result = i;
				this.result.change.coins[i]--;
				break;
			}
		}
		return result;
	}
} // end of class: CEvalAI


// NODE.JS Stuff

var content = '';
process.stdin.resume();
process.stdin.on('data', function (buf) {
	content += buf.toString();
});

process.stdin.on('end', function() {
	var l=0;
	var lines = content.split('\n');
	var line = lines[l++].split(' ');
	var players = parseInt(line[0]);
	var turn = parseInt(line[1]);

	var game=FIGHT.CGame(players,turn);
	game.change = new FIGHT.CChange( lines[l++] );
	for (var v=0;v!=players;++v) {
		game.players[v].coins.coins = game.players[v].coins.decode(lines[v+l]);
	}
	
	var ai=new CWookieAI();
	var coin = ai.pickCoin(game, game.players[turn%players]);
	var move = new FIGHT.CMove(coin,ai.changeToTake);
	
	console.log(move.encode());
	
	
	//console.log(lines);
});
