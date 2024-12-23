;------------------------------------------------------------------
; test.ll - A small example to test loop passes
;------------------------------------------------------------------
; This function sums up the elements of an integer array of size n.

define i32 @sum_array(i32* %arr, i32 %n) {
entry:
  ; Allocate i and sum on the stack
  %i   = alloca i32, align 4
  %sum = alloca i32, align 4

  store i32 0, i32* %i, align 4
  store i32 0, i32* %sum, align 4

  ; Jump to loop condition
  br label %cond

;------------------------------------------------------------------
; Loop condition
;------------------------------------------------------------------
cond:
  ; Load the current iteration index
  %i_val = load i32, i32* %i, align 4
  ; Compare index with n
  %cond_result = icmp slt i32 %i_val, %n
  br i1 %cond_result, label %loop, label %exit

;------------------------------------------------------------------
; Loop body
;------------------------------------------------------------------
loop:
  ; Zero-extend i_val to 64 bits to index into arr
  %idxprom = sext i32 %i_val to i64
  ; Get the pointer to arr[i]
  %elem_ptr = getelementptr inbounds i32, i32* %arr, i64 %idxprom
  ; Load the element from arr[i]
  %elem_val = load i32, i32* %elem_ptr, align 4

  ; Load current sum
  %sum_val = load i32, i32* %sum, align 4
  ; sum_new = sum_val + elem_val
  %sum_new = add nsw i32 %sum_val, %elem_val
  ; Store the updated sum
  store i32 %sum_new, i32* %sum, align 4

  ; Increment i
  %i_next = add nsw i32 %i_val, 1
  store i32 %i_next, i32* %i, align 4
  ; Jump back to the loop condition
  br label %cond

;------------------------------------------------------------------
; Loop exit
;------------------------------------------------------------------
exit:
  ; Load final sum and return it
  %final_sum = load i32, i32* %sum, align 4
  ret i32 %final_sum
}
