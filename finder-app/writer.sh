filesdir=$1
writestr=$2

if [ -z $filesdir ] 
then
  echo "filesdir not specified"
  exit 1
fi

if [ -z $writestr ]
then
  echo "writestr not specified"
  exit 1
fi

if [ ! -d $filesdir ]
then 
  echo "not a dir"
  mkdir -p $(dirname $filesdir)
fi

echo $writestr > $filesdir
