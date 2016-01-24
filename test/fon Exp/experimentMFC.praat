writeInfoLine: "ExperimentMFC"

Read from file: "simplest.ExperimentMFC"
Run
Remove

ex = Read from file: "simplest.ExperimentMFC"
Run
editor: "simplest"
	Close
endeditor

removeObject: ex

appendInfoLine: "OK"
