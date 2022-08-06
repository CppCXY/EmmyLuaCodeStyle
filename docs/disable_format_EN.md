# format disable

Usually there are some files that are not suitable for formatting in the project, or the formatting of some statements in a file requires exceptional manual processing. For such cases, the algorithm library still provides certain support.

## use ---@format

Following the way of emmylua doc, I implemented the way of disable formatting by analyzing comments.

Add `---@format disable-next` above the statement to be disable the formatting in the statement.

E.g:
````lua
---@format disable-next
local t=123

````

If a statement block needs to be disable-format, add `---@format disable` to header of block for disabled formatting.

E.g:
````lua
---@format disable

local d = 123
local cc=2313
local ccc =13

````

## Notice

1. Regardless of whether the formatting is disable or not, the indentation of the statement masked by the format annotation will still be formatted.
2. The unit of masking and formatting is a statement, not an expression. If the format annotation is written as an inline comment, the masking will not take effect.
