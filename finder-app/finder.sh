filesdir=$1
searchstr=$2

if [ -z $filesdir ] 
then
  echo "filesdir not specified"
  exit 1
fi

if [ -z $searchstr ]
then
  echo "searchstr not specified"
  exit 1
fi

if [ ! -d $filesdir ]
then 
  echo "$filesdir is not a directory"
  exit 1
fi

filesfound=$(find -L $filesdir -type f | wc -l)
linesfound=$(grep -R $searchstr $filesdir | wc -l)
echo "The number of files are $filesfound and the number of matching lines are $linesfound"
