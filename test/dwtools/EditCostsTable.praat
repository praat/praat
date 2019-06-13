target = Create Strings as characters: "intention"
source = Create Strings as characters: "execution"
selectObject: source, target
edt = To EditDistanceTable
Draw edit operations
removeObject: edt, target, source
