

# Check if both arguments are specified
if [ $# -lt 2 ]; then
  echo "Error: both parameters (filesdir and searchstr) must be specified"
  exit 1
fi

# Check if filesdir is a directory
if [ ! -d $1 ]; then
  echo "Error: $1 is not a directory"
  exit 1
fi

if [ ! -n $2 ]; then
  echo "Error: $2 is not a string"
  exit 1
fi


# Assign the arguments to variables
filesdir=$1
searchstr=$2

# Count the number of files
num_files=$(find $filesdir -type f | wc -l)

# Count the number of matching lines
num_lines=$(grep -r "$searchstr" $filesdir | wc -l)

# Print the results
echo "The number of files are $num_files and the number of matching lines are $num_lines"

