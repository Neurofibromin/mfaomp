#!/bin/bash

# This script searches for all files ending with .h in the current directory's parent directory, and the parent's subdirectories.
# It then lists the filenames on the console and creates a 'headers.txt' file containing the filename, a separator, the file content, and another separator.
# Afterwards copyright notices are removed

echo "Searching for .h files in the current directory and parent directory/subdirectories..."
echo "------------------------------------------------------------------"
declare -a header_files=()
while IFS= read -r -d $'\0' file; do
    header_files+=("$file")
done < <(find .. -maxdepth 2 -type f -name "*.h" -print0)
echo "Files found in parent directory and its subdirectories (..):"
for file in "${header_files[@]}"; do
    basename "$file"
done
echo "------------------------------------------------------------------"
echo "Printing the full paths stored in the array:"
printf '%s\n' "${header_files[@]}"
echo "------------------------------------------------------------------"
echo "Creating headers.txt with content..."
> headers.txt
if [ ${#header_files[@]} -eq 0 ]; then
    echo "No header files found to add to headers.txt."
else
    for file_path in "${header_files[@]}"; do
        echo "Processing: $(basename "$file_path")" >&2
        basename "$file_path"
        echo "=========="
        cat "$file_path"
        echo "=========="
    done > headers.txt
    echo "headers.txt created successfully."
fi
echo "------------------------------------------------------------------"
if [ -s headers.txt ]; then
    echo "Removing copyright notices from headers.txt..."
    sed -i '/^    mfaomp - Multiple Files At Once Media Player$/,/^\    along with this program\.  If not, see <https:\/\/www\.gnu\.org\/licenses\/>\.$/d' headers.txt
    echo "Copyright notices removed."
else
    echo "Skipping copyright removal, headers.txt is empty."
fi

tree -L 3 .. > headers.txt