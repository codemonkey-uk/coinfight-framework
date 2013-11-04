import sys

def pick_coin(coins):
	for coin, num in sorted(coins.iteritems()):
		if num > 0:
			return coin


def get_change(in_coin, table):
	taken_coins = {1:0, 5:0, 10: 0, 25: 0}
	total_value = 0
	for coin, num in table.iteritems():
		if coin < in_coin - total_value:
			for i in xrange(num):
				if coin < in_coin - total_value:
					taken_coins[coin] += 1
					total_value += coin

	return taken_coins


def print_coins(coins):
	out_list = []
	for coin, num in sorted(coins.iteritems()):
		coin_string = "%sx%s" % (str(num), str(coin))
		out_list.append(coin_string)

	return ','.join(out_list)


def adjust_balance(total_coins, coins, transaction='in'):
	for coin, num in coins.iteritems():
		if transaction == 'in':
			total_coins[coin] += num
		else:
			total_coins[coin] -= num
		
	return total_coins


def eliminated(coins):
	player_out = True
	for coin, num in coins.iteritems():
		if num > 0:
			player_out = False
			break

	return player_out


if __name__ == '__main__':

	i = 0
	input_coins = {}
	#with open('opening.txt') as opening_file:
		#for line in opening_file:
	for line in sys.stdin:
		broken_line = line.strip().split(',')
		
		if len(broken_line) < 4:
			players, turn = broken_line[0].split(' ')
			player_line = int(turn) % int(players) + 1
		
		else:	
			input_coins[i] = {int(in_coins.split('x')[1]): int(in_coins.split('x')[0]) for in_coins in broken_line}
			i += 1 

	table_coins = input_coins[0]
	player_coins = input_coins[player_line]

	play_coin = pick_coin(player_coins)
	player_coins[play_coin] -= 1

	out_print = str(play_coin) + '\n'

	change = get_change(play_coin, table_coins)
	out_print = "%s\n%s" % (str(play_coin), print_coins(change))

	print out_print
