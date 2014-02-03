
(defn parse-change-outer 
	"splits a comma separated string"
	[line]
	(clojure.string/split line #",\s?")
)

(defn parse-change-pair 
	"splits a string '8x1' into a pair of integers"
	[line]
	(map #(Integer. %) (clojure.string/split line #"x"))
)

(defn parse-change 
	"splits '8x1, 9x5, 6x10, 3x25' into ([8 1] [9 5] [6 10] [3 25])"
	[line] 
	(map parse-change-pair
		(parse-change-outer line)
	)
)

(defn encode-change-pair
	"encodes a 2 element list as a NxM string"
	[p]
	(format "%dx%d" (first p) (second p))
)

(defn encode-change
	"encodes a change list as a string"
	[change]
	(if (= (count change) 1) 
		(format "%s"
			(encode-change-pair (first change))
		)	
		(format "%s, %s"
			(encode-change-pair (first change))
			(encode-change (rest change))
		)
	)
)

(defn change-as-values
	"multiplies the quantities by their values in an"
	[change]
	(map #(apply * %) change)
)

(defn change-value
	"Calculates the value of a change-pool"
	[change]
	(apply + (change-as-values change))
)

(defn change-filter
	"Removes coin denominations that the count is zero for"
	[change]
	(filter #(> (first %) 0) change)
)

(defn change-from-inner
	"returns the change-pair that can be taken from the arg pair up to value"
	[value pair]
	; quot for integer division
	(list (min (quot value (second pair)) (first pair)) (second pair))
)

(defn change-from
	"returns an amount of change to take from 'change' for 'value'"
	[value change]
	(let [innr (change-from-inner value (first change))]
		(if (= (count change) 1) 
			(list innr)
			(concat (change-from (- value (apply * innr)) (rest change)) [innr])
		)
	)
)

(defn do-coinfight
	"Do it!"
	[inp]
	
	(def player-count (Integer. (first (clojure.string/split (first inp) #"\s+"))))
	(def turn-number (Integer. (second (clojure.string/split (first inp) #"\s+"))))

	; (println player-count turn-number)
	(def whos-turn (mod turn-number player-count))
	
	(def change-pools (map 
		#(parse-change %)
		(rest inp)
	))

	(def table-change (first change-pools))
	(def player-change (rest change-pools))

	; (println table-change)
	; (println (nth player-change whos-turn))

	; play coin of lowest value
	(def f (change-filter (nth player-change whos-turn)))
	(def play-value (second (first f)))
	(println play-value)
	
	; take some change
	(def tk (reverse (change-from (- play-value 1) table-change)))
	(println (encode-change tk))
)

; creates a variable inp that is a list of lines
(def inp (apply list (clojure.string/split (slurp *in*) #"\n")))

(do-coinfight inp)
