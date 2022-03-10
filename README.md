# SCP: Project Unreal
 SCP: Project Unreal is an Unreal Engine recreation of the game [SCP: Containment breach](http://scpcbgame.com/)
 
 You can find the sourcecode of SCP: Containment Breach at https://github.com/Regalis11/scpcb.   
 There is also a rewrite to a custom engine in progress at https://github.com/juanjp600/scpcb

This is the open-source restart of development for SCP: Project Unreal.
The original version of this project was not set up with source control from the beginning and its codebase was too cluttered and lots of things had to be redone constantly which is why I decided to restart development from scratch.

#### Projectfile and contribution information:
- This project uses Unreal Engine 4.27 with the AMD FSR plugin. [License](https://github.com/RadioArtz/scp-project-unreal/blob/main/Plugins/FSR/license.txt)
- When working on some major system, create an issue to inform others that you are working on it and keep it updated if you stop working on it. This is to avoid two people spending a lot of time just to find out that someone else has been working on the same thing. You can of course still work on things that others are already working on if you want to.
- The project has c++ enabled.
- **No external marketplace assets** are to be used!
- Do **NOT** contribute assets and/or code that don't fall under the [License](https://github.com/RadioArtz/scp-project-unreal/blob/d0a3f758f9d75258092dbfb309da14a6c6b8ec47/License.txt)
- Forks of the project are to follow the same License terms as this Project.

 
### HELP MY PROJECT IS NOT COMPILING!!
Of course it isn't, welcome to C++. But here’s how to fix it:
1.    Make sure to fetch a working version
2.    Delete the “Intermediate” folder inside the project folder
3.    Right-click the project file and click on “Generate Visual Studio project files”
4.    Open the solution file
5.    Right-click “scppu” inside the solution-explorer
6.    Click on “Set as start-up project”
7.    Select “Development Editor” in the combo box located at the top beneath the “Build” menu
8.    Start local debug (sometimes needs multiple tries until it actually works because C++ stuff) 
This should generally fix it. If it doesn't, join the Discord and we'll try to help you out.


#### Discord
-There are two channels on this project's Discord dedicated to development. One is publically avaliable and the other is for verified contributors.
Invite:
https://discord.gg/Zs2UMr8
