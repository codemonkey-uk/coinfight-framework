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

selectMove :: Game -> Move
selectMove game = Move { 
	giveCoin = Coin {
		faceValue = faceValue $ head change,
		quantity = 1 
	},
	takeChange = []
} where change = (playerChange game) !! (currentPlayer game)

processGame :: String -> String
processGame contents = serialiseMove $ selectMove game 
	where game = deserialiseGame $ lines contents

main = do
	contents <- getContents
	putStrLn $ processGame contents