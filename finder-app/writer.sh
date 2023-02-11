

writefile=$1
writestr=$2

# Check if both arguments are specified
if [ $# -lt 2 ]; then
  echo "Error: both parameters (writefile and writestr) must be specified"
  exit 1
fi

if [ ! -n $2 ]; then
  echo "Error: $2 is not a string"
  exit 1
fi

# Create the path if it doesn't exist
pathname=$(dirname "$writefile")
echo $pathname

if [ ! -d "$pathname" ]; then
  mkdir -p "$pathname"
fi

# Create the file with the specified content
echo "$writestr" > "$writefile"

# Check if the file was created successfully
if [ $? -ne 0 ]; then
  echo "Error: Failed to create file '$writefile'."
  exit 1
fi


