#!/usr/bin/env python3

def main():
    print("hello")
    i = input("enter your age: ")
    try:
        if(int(i)<18):
            print("you are under age")
        else:
            print("okay")
    except:
        print("please enter an integer value:")
        
    

print(__doc__)
main()

