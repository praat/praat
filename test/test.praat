writeInfo()
#String function works with Chinese characters in Linux (Ubuntu)
fileName$="02 你好大家好"
a$=right$(fileName$,2)
printline 'a$'

#Regular Expression doesn't work with Chinese characters, but works with English in Linux (Ubuntu)
length=length(fileName$)
a=rindex_regex (fileName$, "\d")
printline a='a'

b$="02 Next time"
b=rindex_regex (b$, "\d")
printline b='b'

#output:家好,a=8,b=2
#correct output should be a and b are both of the value 2
