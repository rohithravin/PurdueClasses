Rohith Ravindranath
PUID: 0028822977
Dan Goldwasser
CS 37300
4th March 2018


I collaborated with Vishal Vasan, Meera Haridasa, Jullian Haresco, and Rohan Saxena.
I affirm that I wrote the solutions in my own words and that I understand the solutions I am submitting.

SPECIAL NOTES:

Usage:

  python3 ID3.py [path to training file (data)] [path to training file (label)] [path to testing file (data)] [path to testing file (label)] [training set percent] [*** optional *** validation set percentage ] [*** optional *** max depth OR min split ]

  [path to training file (data)]  - path to training file data with file extension
  [path to training file (label)]  - path to training file label with file extension
  [path to testing file (data)]  - path to testing file data with file extension
  [path to testing file (label)]  - path to testing file label with file extension
  [training set percent] - value should be between 1 and 100
  [*** optional *** validation set percentage ] - value should be between 1 and 100, valid for any kind of decision tree model except 'vanilla'
  [*** optional *** max depth OR min split ] - describes max_depth value for 'depth' tree type, or minimum split on samples for 'min_split' tree type

  Analysis for the decision tree is in the file called analysis.py

  python3 analysis.py [*** optional *** path to training file (data)] [ *** optional *** path to training file (label)] [ *** optional *** path to testing file (data)] [ *** optional *** path to testing file (label)]

  [ *** optional *** path to training file (data)]  - path to training file data with file extension
  [ *** optional *** path to training file (label)]  - path to training file label with file extension
  [ *** optional *** path to testing file (data)]  - path to testing file data with file extension
  [ *** optional *** path to testing file (label)]  - path to testing file label with file extension

  All 4 four arguments must be provided together or none at all, if none is provided then the program will assume the files are stored in the same directory as the python file and assume the files are:

  TRAINING_FILE_DATA = 'titanic-train.data'
  TRAINING_FILE_LABEL = 'titanic-train.label'
  TESTING_FILE_DATA = 'titanic-test.data'
  TESTING_FILE_LABEL = 'titanic-test.label'

  All the plots from the analysis.py are saved as a .png extension in the same directory as the python file
