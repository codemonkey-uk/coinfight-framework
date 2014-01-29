
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
	"Calculates the value of a change-pool"
	[change]
	(apply + (change-as-values change))
)

(defn change-filter
	"Removes coin denominations that the count is zero for"
	[change]
	(filter #(> (first %) 0) change)
)

(defn do-coinfight
	"Do it!"
	[inp]
	
	(def player-count (Integer. (first (clojure.string/split (first inp) #"\s+"))))
	(def turn-number (Integer. (second (clojure.string/split (first inp) #"\s+"))))

	; (println player-count turn-number)
	(def whos-turn (mod turn-number player-count))
	
	(def change-pools (map 
		#(change-filter (parse-change %))
		(rest inp)
	))

	(def table-change (first change-pools))
	(def player-change (rest change-pools))

	; (println table-change)
	; (println (nth player-change whos-turn))

	; play coin of lowest value
	(def f (change-filter (nth player-change whos-turn)))
	(println (second (first f)))
	
	; take no change
	(println "0x1, 0x5, 0x10, 0x25")	
)

;(defn foo "" [line] (println ">" line))
;(doseq 
;	[line (clojure.string/split (slurp *in*) #"\n")] (foo line)	
;)

; creates a variable inp that is a list of lines
(def inp (apply list (clojure.string/split (slurp *in*) #"\n")))

(do-coinfight inp)
