
(defn parse-change-outer 
	"splits a comma separated string"
	[line]
	(clojure.string/split line #",\s?")
)

(defn parse-change-inner 
	"splits a string '8x1' into a pair of integers"
	[line]
	(map #(Integer. %) (clojure.string/split line #"x"))
)

(defn parse-change 
	"splits '8x1, 9x5, 6x10, 3x25' into ([8 1] [9 5] [6 10] [3 25])"
	[line] 
	(map parse-change-inner
		(parse-change-outer line)
	)
)

(defn change-as-values
	"multiplies the quantities by their values in an"
	[change]
	(map #(apply * %) change)
)

(defn change-value
	""
	[change]
	(apply + (change-as-values change))
)

(defn change-filter
	""
	[change]
	(filter #(> (first %) 0) change)
)

(def inp (list 
	"8x1, 9x5, 6x10, 3x25", 
	"1x1, 0x5, 0x10, 0x25",
	"0x1, 0x5, 0x10, 0x25",
	"3x1, 0x5, 0x10, 0x25")
)

(def change-pools (map 
	#(change-filter (parse-change %))
	inp
))

(def table-change (first change-pools))
(def player-change (rest change-pools))

(println table-change)
(println player-change)


