import sys, getopt, os

def main(argv):
    inputDir = ''
    outputDest = ''
    assetFolder = ''
    isPack = False
    opts, args = getopt.getopt(argv, "hi:o:a:p", ["help","input=","output=","asset=","pack"])
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print("Usage: helloui [command]\n\nCommand List:\n\n-i --input [source code to compile]\tthe .hui source code you want to compile\n-o --output [output destination]\tthe output executable, or the output package file name without extension\n-a --asset [asset folder]\t\tdefault to asset folder in the same directory\n-p --pack\t\t\t\tif the output is packaged as hux, default to false\n")
            sys.exit()
        elif opt in ("-i", "--input"):
            fileName = arg.split(".")
            if len(fileName) < 2 or fileName[len(fileName)-1] != "hui":
                print("Invalid file name")
                sys.exit()
            if not(os.path.exists(arg)):
                print("File does not exist")
                sys.exit()
            inputDir = arg
        elif opt in ("-o", "--output"):
            if len(arg.split(".")) > 1:
                print("Invalid output file name")
                sys.exit()
            outputDest = arg
        elif opt in ("-a", "--asset"):
            if len(arg.split(".")) > 1:
                print("Invalid asset folder name")
                sys.exit()
            assetFolder = arg
        elif opt in ("-p", "--pack"):
            isPack = True
    
    with open(inputDir) as f:
        lines = f.readlines()
        print(lines)

if __name__ == "__main__":
    main(sys.argv[1:])
