import Data.Char
import Data.List
import Data.List.Split

data Coin = Coin {
	faceValue :: Int,
	quantity :: Int
} deriving( Show )

data Change = Change {
	coins :: [Coin]
} deriving( Show )

data Game = Game {
	playerCount :: Int,
	turn :: Int,
	tableChange :: Change,
	playerChange :: [Change]
} deriving( Show )

deserialiseCoin :: String -> Coin
deserialiseCoin str = Coin {
	faceValue = read $ values !! 1,
	quantity = read $ values !! 0
} where values = splitOn "x" str

deserialiseChange :: String -> Change
deserialiseChange str = Change {
	coins = map deserialiseCoin values
} where values = splitOn "," str

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
	give :: Coin,
	takeCoin :: Change
} deriving( Show )

serialiseCoin :: Coin -> String
serialiseCoin coin = (show $ quantity coin) ++ "x" ++ (show $faceValue coin)

serialiseChange :: Change -> String
serialiseChange c = intercalate ", " $ map serialiseCoin $ coins c

serialiseMove :: Move -> String
serialiseMove move = (serialiseCoin $ give move) ++ "\n" ++ (serialiseChange $ takeCoin move)

processGame :: String -> String
processGame contents = head $ lines contents

main = do
	contents <- getContents
	putStrLn $ processGame contents