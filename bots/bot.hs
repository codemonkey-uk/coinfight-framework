import Data.Char
import Data.List
import Data.List.Split

data Coin = Coin {
	faceValue :: Int,
	quantity :: Int
} deriving( Show )

data Game = Game {
	playerCount :: Int,
	turn :: Int,
	tableChange :: [Coin],
	playerChange :: [[Coin]]
} deriving( Show )

deserialiseCoin :: String -> Coin
deserialiseCoin str = Coin {
	faceValue = read $ values !! 1,
	quantity = read $ values !! 0
} where values = splitOn "x" str

deserialiseChange :: String -> [Coin]
deserialiseChange str = [ x | x <- map deserialiseCoin values, quantity x > 0 ]
	where values = splitOn "," str

deserialiseGame :: [String] -> Game
deserialiseGame l = Game {
	playerCount = read $ game_header !! 0, 
	turn = read $ game_header !! 1,
	tableChange = deserialiseChange $ head change,
	playerChange = map deserialiseChange (tail change)
} where 
	game_header = words $ head l
	change = tail l

data Move = Move {
	giveCoin :: Coin,
	takeChange :: [Coin]
} deriving( Show )

serialiseCoin :: Coin -> String
serialiseCoin coin = (show $ quantity coin) ++ "x" ++ (show $faceValue coin)

serialiseChange :: [Coin] -> String
serialiseChange c = intercalate ", " $ map serialiseCoin $ c

serialiseMove :: Move -> String
serialiseMove move = (show $ faceValue $ giveCoin move) ++ "\n" ++ (serialiseChange $ takeChange move)

currentPlayer :: Game -> Int
currentPlayer game = mod (turn game) (playerCount game)

coinValue :: Coin -> Int
coinValue coin = (faceValue coin) * (quantity coin)

-- add a coin to a change pool, 
-- increasing the quantity of an existing coin if it's face value matches
addCoin :: Coin -> [Coin] -> [Coin]
addCoin coin [] = [coin]
addCoin coin (x:xs) = 
	if (faceValue x) == (faceValue coin)
	then Coin{ faceValue=(faceValue x), quantity=(quantity x)+(quantity coin) }:xs
	else x:addCoin coin xs

-- Returns change < value from 
takeChangeFrom :: Int -> [Coin] -> [Coin]
takeChangeFrom v [] = []
takeChangeFrom v (x:xs) =
	if (quantity took) < 1
	then (takeChangeFrom v xs)
	else took:takeChangeFrom (v-(coinValue took)) xs
	where took = Coin{ 
		faceValue= (faceValue x), 
		quantity = minimum [(quot (v-1) (faceValue x)), (quantity x)]
	}

selectMove :: Game -> Move
selectMove game = Move { 
	giveCoin = g,
	takeChange = takeChangeFrom (coinValue g) (tableChange game)
} where 
	change = (playerChange game) !! (currentPlayer game)
	g = Coin {
		faceValue = faceValue $ head change,
		quantity = 1 
	}

processGame :: String -> String
processGame contents = serialiseMove $ selectMove game 
	where game = deserialiseGame $ lines contents

main = do
	contents <- getContents
	putStrLn $ processGame contents