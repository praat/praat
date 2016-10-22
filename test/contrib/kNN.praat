a1 = 0
a2 = 0
a3 = 0
for i from 1 to 30
	Create iris example...  0 0
	select FFNet 4-3
	select Pattern iris
	plus Categories iris
	To KNN Classifier: "Classifier", "Sequential"
	temp = Get accuracy estimate: "Leave one out", 10, "Inverse squared distance"
	a1 = a1 + (temp - a1) / i
	Shuffle
	temp = Get accuracy estimate: "10-fold cross-validation", 10, "Inverse squared distance"
	a2 = a2 + (temp - a2) / i
	Prune... 1 1 10
	select Pattern iris
	plus Categories iris
	plus KNN Classifier
	temp = Evaluate: 10, "Inverse squared distance"
	a3 = a3 + (temp - a3) / i
	select FFNet 4-3
	plus Pattern iris
	plus Categories iris
	plus KNN Classifier
	Remove
endfor
result = (a1 + a2 + a3) / 3
if result > 96
	printline OK
else
	printline Failed
endif
