// jsFIGHT.js
// JavaScript library for Coin Fight game logic

var COIN_COUNT = 4;

var PENNY = 1;
var NICKEL = 5;
var DIME = 10;
var QUARTER = 25;

var COINLIST = [PENNY, NICKEL, DIME, QUARTER];
		
var INITAL_PLAYER_COINS = [ 4, 3, 2, 1 ];
var EMPTY_CHANGE = [ 0, 0, 0, 0 ];
	
function CChange( coins )
{
	this.generateTXT = function() {
		var result = "";
	
		for (i = 0; i < this.coins.length; ++i)
			if (this.coins[i]>0)
			{
				if (result.length>0)
					result += ', ';
				result += this.coins[i] + 'x ' + COINNAMES[i]; 
			}
		if (result.length>0)
			result += '.';
		return result;
	}
	
	this.decode = function(s)
	{
		var coins_in=s.split(',');
		var result=new Array();
		coins_in.forEach( function(coin) {
			var e=coin.split('x');
			var n=parseInt(e[0]);
			var v=parseInt(e[1]);
			result[COINLIST.indexOf(v)]=n;
		});	
		return result;	
	}
	
	this.encode = function()
	{
		var result='';
		for (var i=0;i!=this.coins.length;++i)
		{
			if (i>0) result += ", ";
			result += this.coins[i] + 'x' + COINLIST[i];
		}
		result+='\n';
		return result;
	}
		
	if (typeof(coins)=="string")
	{
		this.coins = this.decode(coins);
	}
	else
	{
		this.coins = coins.slice();
	}
	
	this.copy = function() { return new CChange( this.coins ); }
	this.getCount = function( i ) { return this.coins[i]; }
	this.getTotalCount = function() { 
		var result = 0;
		for (i = 0; i < this.coins.length; ++i)
			result += this.coins[i]; 
		return result;
	}
	this.getCoinTypeCount = function() { 
		var result = 0;
		for (i = 0; i < this.coins.length; ++i)
			if (this.coins[i]>0) result ++; 
		return result;
	}
	this.getValue = function( i ) { return this.coins[i]*COINLIST[i]; }
	this.getTotalValue = function() { 
		var result = 0;
		for (i = 0; i < this.coins.length; ++i)
			result += this.coins[i]*COINLIST[i]; 
		return result;
	}
	this.empty = function() {
		for (i = 0; i < this.coins.length; ++i)
			if (this.coins[i] > 0) 
				return false;
		return true;
	}
	this.add = function( rhs ) {
		if (typeof rhs == "number")
			this.coins[rhs] ++;
		else
			for (i = 0; i < this.coins.length; ++i)
				this.coins[i] += rhs.coins[i];
	}
	this.subtract = function( rhs ) {
		if (typeof rhs == "number")
			this.coins[rhs] --;
		else
			for (i = 0; i < this.coins.length; ++i)
				this.coins[i] -= rhs.coins[i];
	}
	this.insertCoins = function( index, amount ) {
		this.coins[index] += amount;
	}
	this.hasCoins = function( rhs ) {
		for (i = 0; i < this.coins.length; ++i)
			if (this.coins[i] < rhs.coins[i])
				return false;
		return true;
	}
	
	return this;
}

function CMove(coinID, change)
{
	this.coin = coinID;

	if (change)
		this.change = new CChange( change.coins );
	else
		this.change = new CChange( EMPTY_CHANGE );

	this.encode = function() {
		var result="";
		result += COINLIST[this.coin];
		result += '\n';
		result += this.change.encode();	
		return result;	
	}
	
	return this;
}

function CPlayer()
{
	this.coins = new CChange( INITAL_PLAYER_COINS );

	this.empty = function() { 
		return this.coins.empty();
	}
	this.giveChange = function( change ) {
		this.coins.add( change );
	}
	
	return this;
}

function CGame(num_players, movesPlayed)
{
	this.players = Array();
	for (var i=0;i!=num_players;++i)
		this.players[i] = new CPlayer(i);
		
	this.turn = movesPlayed%num_players;
	this.movesPlayed = movesPlayed;

	this.move = new CMove(-1);
	this.change = new CChange( EMPTY_CHANGE );

	this.encode = function()
	{
		var result = "";
		result += this.players.length;
		result += ' ';
		result += this.movesPlayed;
		result += '\n';
		result += this.change.encode();
		this.players.forEach( function(player) {
			result += player.coins.encode();
		});
	
		return result;
	}
	this.decode = function( s )
	{	
		
	}
	
	this.validateMove = function() {
		if (this.move.coin==-1)
			return false;
		if (this.players[this.turn].coins.getCount(this.move.coin)<1)
			return false;
		return this.change.hasCoins(this.move.change);
	}

	this.playMove = function() {
		if (this.validateMove())
		{
			this.movesPlayed++;
			this.players[this.turn].coins.subtract(this.move.coin);
			this.change.add(this.move.coin);
		
			this.change.subtract( this.move.change );
			this.players[this.turn].giveChange( this.move.change );
		
			if (this.players[this.turn].empty())
			{				
				// end game
			}
			else
			{
				// flip turn
				this.turn = 1-this.turn;
			
				this.startTurn( msg );
			}
		}
		else
		{
			// errpr
		}
	}

	this.canTakeChange = function (coin) { 
		var result = false;
		if (this.move.coin!=-1)
		{
			var player = this.players[this.turn];
			var value = COINLIST[this.move.coin];

			for (var i = 0; i < this.move.change.coins.length; ++i){		
				value -= this.move.change.coins[i] * COINLIST[i];
			}
		
			if (typeof coin != "number")
			{
				for (var i = 0; i < this.change.coins.length; ++i){
					if (this.change.coins[i] > this.move.change.coins[i] && COINLIST[i] < value) {
						result=true;
						break;
					}
				}
			}
			else
			{
				if (this.change.coins[coin] > this.move.change.coins[coin] && COINLIST[coin] < value) {
					result = true;
				}
			}
		}
		return result;
	}
	
	return this;
}

module.exports = {

	COIN_COUNT: COIN_COUNT,

	PENNY: PENNY,
	NICKEL: NICKEL,
	DIME: DIME,
	QUARTER: QUARTER,

	COINLIST: COINLIST,
		
	INITAL_PLAYER_COINS: INITAL_PLAYER_COINS,
	EMPTY_CHANGE: EMPTY_CHANGE,

	CChange: CChange,

	CMove: CMove,

	CPlayer: CPlayer,
	
	CGame: CGame
};

