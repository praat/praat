# test_FFNet.praat
# djmw 20110512

printline test FFNet

Create iris example... 0 0
ffnet = selected ("FFNet")
pattern = selected ("Pattern")
cat = selected ("Categories")

ntries = 10
error1 = Get total costs... Minimum-squared-error
tab = Create Table with column names... results ntries costsb  costsa fc 

for i to ntries
	select ffnet
	Reset... 0.1
	plus pattern
	plus cat
	costsb = Get total costs... Minimum-squared-error
	Learn...  200 1e-7 Minimum-squared-error
	costsa = Get total costs... Minimum-squared-error
	select ffnet
	plus pattern
	cati = To Categories... Winner-takes-all
	plus cat
	fd = Get fraction different
	fc = 1 -fd
	select tab
	Set numeric value... i costsb costsb
	Set numeric value... i costsa costsa
	Set numeric value... i fc fc
	select cati
	Remove
endfor

select tab
mean = Get mean... fc
assert fc > 0.97
plus ffnet
plus pattern
plus cat
Remove

@test_openSave

printline FFNet ok

procedure test_openSave
	.ffnet_read= Read from file: "iris_4-2-3-3.FFNet"
	Create iris example: 2, 3
	.ffnet = selected ("FFNet")
	.pattern = selected ("Pattern")
	.categories = selected ("Categories")

	selectObject: .ffnet
	Save as binary file: "kanweg.FFNet"
	.ffnet_read2 = Read from file: "kanweg.FFNet"
	# are they the same ??

	selectObject: .ffnet_read, .pattern, .categories
	.costs[1] = Get total costs: "Minimum-squared-error"
	selectObject: .ffnet_read, .pattern, .categories
	Learn: 100, 1e-7, "Minimum-squared-error"
	.costs[2] = Get total costs: "Minimum-squared-error"
	assert .costs[1] >= .costs[2]

	removeObject: 	.ffnet, .ffnet_read, .ffnet_read2, .pattern, .categories
endproc


