/*login: lixx3527 and 
* date: October/7/2015
* name: Cuiqing Li, full_name2
* id: 5028341, id_for_second_name */

Description of my program:
    Basically, I let my program identify whether it will do encoding job or decoding job. For example,if we need to encode my job, after creating a directory and a reportfile within it, I will execute function encodefile(). Next, I will let getaccess to encodefile and enter into it. After entering it, I will open file one by one. We need to read data in those files and encode them into a new file in output directory we just created it. If we encountered a subdirectory, we neeed to enter into that subdirectory(change start_path and target_path) to continue to read data from files in the start_path, and read data to the coressponding filesin  the target_path(Over here, I use a recursive ways to finish the job, since maybe the directory has many subdirectories). Hence, using recursive ways can be very convenient for us to handle the problem. After finishing the job, I sort the data in the file so that I can make the file become organized in a ordered way.
     We can use same way to finish the procedures above if we need to decode files in the decotest directory. 
