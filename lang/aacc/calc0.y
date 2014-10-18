# Grammar

		pgm		list pgm 
		pgm		list

:finish		list		expr			:result

		expr		constant		:pass0
:add		expr		expr add expr		:doadd
:sub		expr		expr sub expr		:dosub
:mul		expr		expr mul expr		:domul
:div		expr		expr div expr		:dodiv
:neg		expr		sub expr		:doneg
		expr		lparen expr rparen	:doparen

# Conflict resolution

# This occurs when we have an expression and the next token is a sub
# Either we have one expression 'expr-expr'
# or we have two expressions 'expr' and '-expr'
(shift finish :sub) shift

# Precidence table
(shift add :add) add
(shift add :sub) add
(shift add :mul) shift
(shift add :div) shift

(shift sub :add) sub
(shift sub :sub) sub
(shift sub :mul) shift
(shift sub :div) shift

(shift mul :add) mul
(shift mul :sub) mul
(shift mul :mul) mul
(shift mul :div) mul

(shift div :add) div
(shift div :sub) div
(shift div :mul) div
(shift div :div) div

(shift neg :add) neg
(shift neg :sub) neg
(shift neg :mul) neg
(shift neg :div) neg
