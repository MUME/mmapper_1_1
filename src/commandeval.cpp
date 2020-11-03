/***************************************************************************
*   Copyright (C) 2003 by Marek Krejza                                    *
*   mkrejza@cbox.cz                                                       *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "commandeval.h"
#include "pref.h"
#include "defs.h"

CommandEval& Parser() {
	static CommandEval eval;
	return eval;
};


CommandEval::CommandEval(QObject *parent, const char *name)
: QObject(parent, name) {
	readingRoomDesc = FALSE;
	descriptionready = FALSE;
	roomName="";
	roomDesc="";
	parsedRoomDesc="";
	roomFlags = 0;

	following = FALSE;
	//followName = "";
	followDir = dir_unknown;
}


CommandEval::~CommandEval() {}

void CommandEval::parseAnswers(QString& str) {

	QString pattern;
	Command* c=NULL;
	QRegExp rx;
	bool matches = FALSE;

	if (str.startsWith("Brief mode on")){
		Config().m_brief = TRUE;   
		emit sendToClient("[MMapper] brief mode on.\n");
		return;
	}

	if (str.startsWith("Brief mode off")){
		Config().m_brief = FALSE;   
		emit sendToClient("[MMapper] brief mode off.\n");
		return;
	}

	if (str.startsWith("You now follow")){
		following = TRUE;   
		emit sendToClient("[MMapper] follow mode on.\n");
		return;
	}

	if (following) {
		if (str=="You will not follow anyone else now."){
			following = FALSE;   
			emit sendToClient("[MMapper] follow mode off.\n");
			return;
		}

		if (str.startsWith("You follow")){
			switch (followDir) {
		  case dir_north:   queue.enqueue(new Command(id_north));break;     
		  case dir_south:   queue.enqueue(new Command(id_south));break;     
		  case dir_east:    queue.enqueue(new Command(id_east));break;     
		  case dir_west:    queue.enqueue(new Command(id_west));break;     
		  case dir_up:      queue.enqueue(new Command(id_up));break;     
		  case dir_down:    queue.enqueue(new Command(id_down));break;     				  
		  case dir_unknown: queue.enqueue(new Command(id_unknown));break;     
			}
		}

		if (str.contains("leaves north")) followDir = dir_north;
		else
			if (str.contains("leaves south")) followDir = dir_south;
			else
				if (str.contains("leaves east"))  followDir = dir_east;
				else
					if (str.contains("leaves west"))  followDir = dir_west;
					else
						if (str.contains("leaves up"))    followDir = dir_up;  
						else
							if (str.contains("leaves down"))  followDir = dir_down;
							else
								if (str.contains("leave north")) followDir = dir_north;
								else
									if (str.contains("leave south")) followDir = dir_south;
									else
										if (str.contains("leave east"))  followDir = dir_east;
										else
											if (str.contains("leave west"))  followDir = dir_west;
											else
												if (str.contains("leave up"))    followDir = dir_up;  
												else
													if (str.contains("leave down"))  followDir = dir_down;
	}


	// parse regexps which cancel last char move
	for ( QStringList::Iterator it = Config().m_moveCancelPatterns.begin(); 
		it != Config().m_moveCancelPatterns.end(); ++it ) {
			pattern = *it; 		
			if (pattern[0] != '#') continue;
			switch ((int)((QChar)pattern[1])){
		case 33:  // !
			rx.setPattern(pattern.remove(0,2));
			if (rx.exactMatch(str)) matches = TRUE;
			break;
		case 60:;  // <
			if (str.startsWith(pattern.remove(0,2))) matches = TRUE;
			break;
		case 61:;  // =
			if ( str==(pattern.remove(0,2)) ) matches = TRUE;
			break;
		case 62:;  // >
			if (str.endsWith(pattern.remove(0,2))) matches = TRUE;
			break;
		case 63:;  // ?
			if (str.contains(pattern.remove(0,2))) matches = TRUE;
			break;
		default:continue;
			}
			if (matches) {
				if((c=queue.dequeue())) delete c;
				emit showPath(queue);
				return;   
				//break;   
			}
		}	

		// parse regexps which force char to move and map to sync
		for ( QStringList::Iterator it = Config().m_moveForcePatterns.begin();
			it != Config().m_moveForcePatterns.end(); ++it ) {
				pattern = *it;			
				if (pattern[0] != '#') continue;
				switch ((int)((QChar)pattern[1])){
		case 33:  // !
			rx.setPattern(pattern.remove(0,2));
			if(rx.exactMatch(str)) matches = TRUE;
			break;
		case 60:;  // <
			if(str.startsWith(pattern.remove(0,2))) matches = TRUE;
			break;
		case 61:;  // =
			if(str==(pattern.remove(0,2)) ) matches = TRUE;
			break;
		case 62:;  // >
			if(str.endsWith(pattern.remove(0,2))) matches = TRUE;
			break;
		case 63:;  // ?
			if(str.contains(pattern.remove(0,2))) matches = TRUE;
			break;
		default:continue;
				}
				if (matches) {
					queue.enqueue(new Command(id_unknown));
					emit showPath(queue);
					return;   
					//break;   
				}
			}	
}

QString CommandEval::parseClientCommand(QString& command) {
	QString str = command;
	str=str.simplifyWhiteSpace();

	DoorActionType daction;
	bool dooraction = FALSE;

	if (str.contains("$$DOOR_N$$")){emit genericDoorCommand(command, dir_north,queue); return "";}
	if (str.contains("$$DOOR_S$$")){emit genericDoorCommand(command, dir_south,queue); return "";}
	if (str.contains("$$DOOR_E$$")){emit genericDoorCommand(command, dir_east,queue); return "";}
	if (str.contains("$$DOOR_W$$")){emit genericDoorCommand(command, dir_west,queue); return "";}
	if (str.contains("$$DOOR_U$$")){emit genericDoorCommand(command, dir_up,queue); 	return "";}
	if (str.contains("$$DOOR_D$$")){emit genericDoorCommand(command, dir_down,queue); return "";}
	if (str.contains("$$DOOR$$"))  {emit genericDoorCommand(command, dir_unknown,queue); return "";}

	if (str.startsWith("_setdoor")){
		if (str.section(" ",1,1)=="n") emit setDoorCommand(str.section(" ",2,2), dir_north, queue);
		if (str.section(" ",1,1)=="s") emit setDoorCommand(str.section(" ",2,2), dir_south, queue);
		if (str.section(" ",1,1)=="e") emit setDoorCommand(str.section(" ",2,2), dir_east, queue);
		if (str.section(" ",1,1)=="w") emit setDoorCommand(str.section(" ",2,2), dir_west, queue);
		if (str.section(" ",1,1)=="u") emit setDoorCommand(str.section(" ",2,2), dir_up, queue);
		if (str.section(" ",1,1)=="d") emit setDoorCommand(str.section(" ",2,2), dir_down, queue);
		return "";
	}

	if (str.startsWith("_open"))   {dooraction = TRUE; daction = door_open;}
	else
		if (str.startsWith("_close"))  {dooraction = TRUE; daction = door_close;}
		else
			if (str.startsWith("_lock"))   {dooraction = TRUE; daction = door_lock;}
			else
				if (str.startsWith("_unlock")) {dooraction = TRUE; daction = door_unlock;}
				else
					if (str.startsWith("_pick"))   {dooraction = TRUE; daction = door_pick;}
					else
						if (str.startsWith("_rock"))   {dooraction = TRUE; daction = door_rock;}
						else
							if (str.startsWith("_bash"))   {dooraction = TRUE; daction = door_bash;}
							else
								if (str.startsWith("_break"))   {dooraction = TRUE; daction = door_break;}
								else
									if (str.startsWith("_block"))   {dooraction = TRUE; daction = door_block;}

									if (dooraction) {
										if (str.endsWith(" n")) 
											emit performDoorCommand(dir_north, daction, queue);
										else
											if (str.endsWith(" s")) 
												emit performDoorCommand(dir_south, daction, queue);
											else
												if (str.endsWith(" e")) 
													emit performDoorCommand(dir_east, daction, queue);
												else
													if (str.endsWith(" w")) 
														emit performDoorCommand(dir_west, daction, queue);
													else
														if (str.endsWith(" u")) 
															emit performDoorCommand(dir_up, daction, queue);
														else
															if (str.endsWith(" d")) 
																emit performDoorCommand(dir_down, daction, queue);
															else
																emit performDoorCommand(dir_unknown, daction, queue);
										return "";
									}

									if (str=="n" || str=="north"){
										queue.enqueue(new Command(id_north));
										emit showPath(queue);
										return command;
									}
									if (str=="s" || str=="south"){
										queue.enqueue(new Command(id_south));
										emit showPath(queue);
										return command;
									}
									if (str=="e" || str=="east"){
										queue.enqueue(new Command(id_east));
										emit showPath(queue);
										return command;
									}
									if (str=="w" || str=="west"){
										queue.enqueue(new Command(id_west));
										emit showPath(queue);
										return command;
									}
									if (str=="u" || str=="up"){
										queue.enqueue(new Command(id_up));
										emit showPath(queue);
										return command;
									}
									if (str=="d" || str=="down"){
										queue.enqueue(new Command(id_down));
										emit showPath(queue);
										return command;
									}
									if (str=="l" || str=="look"){
										queue.enqueue(new Command(id_look));
										return command;
									}
									if (str=="exa" || str=="examine"){
										queue.enqueue(new Command(id_examine));
										return command;
									}
									if (str.startsWith("scout")) {
										queue.enqueue(new Command(id_scout));
										return command;
									}
									if (str=="_back") {
										emptyQueue();
										emit sendToClient("OK.\n");
										return "\r\n";//NULL;
									}
									if (str=="_sync") {
										emptyQueue();
										emit sendToClient("OK.\n");
										emit syncCommand(roomName, roomDesc, parsedRoomDesc, roomFlags, promptFlags );
										return "\r\n";//NULL;
									}
									if (str=="_set") {
										emptyQueue();
										emit sendToClient("OK.\n");
										emit setInfoCommand(roomName, roomDesc, roomFlags, promptFlags);
										return "\r\n";//NULL;
									}
									if (str=="_clear") {
										emit sendToClient("OK.\n");
										emit resetCommand();
										return "\r\n";//NULL;
									}
									if (str=="_printdesc") {
										emit sendToClient(roomDesc);
										emit sendToClient("OK.\n");
										return "\r\n";//NULL;
									}
									if (str=="_printparsed") {
										emit sendToClient(parsedRoomDesc/*parseDesc(roomDesc)*/);
										emit sendToClient("OK.\n");
										return "\r\n";//NULL;
									}
									if (str=="_edit") {
										emit sendToClient("OK.\n");
										emit setEditMode();
										return "\r\n";//NULL;
									}
									if (str=="_play") {
										emit sendToClient("OK.\n");
										emit setPlayMode();
										return "\r\n";//NULL;
									}
									if (str=="_off") {
										emit sendToClient("OK.\n");
										emit setFollowMode();
										return "\r\n";//NULL;
									}
									if (str=="_help") {
										emit sendToClient("\nMMapper help:\n-------------\n");

										emit sendToClient("\nStandard MUD commands:\n");
										emit sendToClient("  Move commands: [n,s,...] or [north,south,...]\n");
										emit sendToClient("  Sync commands: [exa,l] or [examine,look]\n");

										emit sendToClient("\nManage prespammed command que:\n");
										emit sendToClient("  _back        - delete prespammed commands from que\n");

										emit sendToClient("\nSynchronization commands:\n");
										emit sendToClient("  _sync        - tries to synchronize mmapper with MUD\n");

										emit sendToClient("\nMode switching commands:\n");
										emit sendToClient("  _edit        - switches mmapper to edit mode\n");
										emit sendToClient("  _play        - switches mmapper to play mode\n");
										emit sendToClient("  _off         - switches mmapper to follow mode\n");

										emit sendToClient("\nDescription commands:\n");
										emit sendToClient("  _printdesc   - prints current room description\n");
										emit sendToClient("  _printparsed - the same as previous, but without moveable items\n");
										emit sendToClient("  _set         - set room info (and mark room as visited)\n");
										emit sendToClient("  _clear       - clear room info (room has to be visited again)\n");

										emit sendToClient("\nDoor commands:\n");
										emit sendToClient("  _open   [n,s,e,w,u,d]   - open door [dir]\n");
										emit sendToClient("  _close  [n,s,e,w,u,d]   - close door [dir]\n");
										emit sendToClient("  _lock   [n,s,e,w,u,d]   - lock door [dir]\n");
										emit sendToClient("  _unlock [n,s,e,w,u,d]   - unlock door [dir]\n");
										emit sendToClient("  _pick   [n,s,e,w,u,d]   - pick door [dir]\n");
										emit sendToClient("  _rock   [n,s,e,w,u,d]   - throw rock door [dir]\n");
										emit sendToClient("  _bash   [n,s,e,w,u,d]   - bash door [dir]\n");
										emit sendToClient("  _break  [n,s,e,w,u,d]   - cast 'break door' door [dir]\n");
										emit sendToClient("  _block  [n,s,e,w,u,d]   - cast 'block door' door [dir]\n");

										emit sendToClient("\n");
										emit sendToClient("  _setdoor [n,s,e,w,u,d] doorname   - sets door name in direction [dir]\n");

										emit sendToClient("\nDoor variables:\n");
										emit sendToClient("  $$DOOR_N$$   - secret name of door leading north\n");
										emit sendToClient("  $$DOOR_S$$   - secret name of door leading south\n");
										emit sendToClient("  $$DOOR_E$$   - secret name of door leading east\n");
										emit sendToClient("  $$DOOR_W$$   - secret name of door leading west\n");
										emit sendToClient("  $$DOOR_U$$   - secret name of door leading up\n");
										emit sendToClient("  $$DOOR_D$$   - secret name of door leading down\n");
										emit sendToClient("  $$DOOR$$     - the same as 'exit'\n");

										emit sendToClient("\nHelp commands:\n");
										emit sendToClient("  _help   - this help text\n");

										emit sendToClient("\n");

										return "\r\n";//NULL;
									}
									return command;
}


QString CommandEval::parseRoomName(QString& str) {

	QString col=Config().m_roomNameColor;
	col=col.prepend(ESC);
	QString name="";

	if ((index=str.find(col))!=-1) {
		index+=col.length();

		name=str;
		name.remove(0,index);
		if((index=name.find(ESC))!=-1) {
			name.remove(index,name.length()-index);
		}
	}
	return name;
}

/*
QString CommandEval::parseDesc(const QString& str) {
tmpstr = str;
tmpstr = removeAnsiMarks(tmpstr);
QString pattern;
int line=0;

QString output="";
bool matches=FALSE;
patternlist = Config().m_descPatterns;
list=list.split("\n",tmpstr);

for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
if ( (++line < 3) || !((*it).endsWith(".")) ) 
{output+=(*it+"\n");continue;}

for ( QStringList::Iterator pit = patternlist.begin();
pit != patternlist.end(); ++pit ) {
pattern = *pit;

if ((pattern)[0] != '#') continue;

switch ((int)((QChar)pattern[1])){
case 33:  // !
rx.setPattern(pattern.remove(0,2));
if (rx.exactMatch(*it)) matches = TRUE;
break;
case 60:;  // <
if ((*it).startsWith(pattern.remove(0,2))) matches = TRUE;
break;
case 61:;  // =
if ( (*it)==(pattern.remove(0,2)) ) matches = TRUE;
break;
case 62:;  // >
if ((*it).endsWith(pattern.remove(0,2))) matches = TRUE;
break;
case 63:;  // ?
if ((*it).contains(pattern.remove(0,2))) matches = TRUE;
break;
default:continue;
}
if (matches) break;
}
if (!matches) output+=(*it+"\n");
else break;
}

return output;
}
*/

bool CommandEval::nonStandardDesc(const QString& str) {
	QString pattern;    
	patternlist = Config().m_endDescPatterns;

	for ( QStringList::Iterator pit = patternlist.begin(); pit != patternlist.end(); ++pit ) {
		pattern = *pit;
		if (pattern[0] != '#') continue;
		switch ((int)((QChar)pattern[1])){
		case 33:  // !
			rx.setPattern(pattern.remove(0,2));
			if (rx.exactMatch(str)) return TRUE;
			break;
		case 60:;  // <
			if (str.startsWith(pattern.remove(0,2))) return TRUE;
			break;
		case 61:;  // =
			if (str==(pattern.remove(0,2)) ) return TRUE;
			break;
		case 62:;  // >
			if (str.endsWith(pattern.remove(0,2))) return TRUE;
			break;
		case 63:;  // ?
			if (str.contains(pattern.remove(0,2))) return TRUE;
			break;
		default:;
		}
	}
	return FALSE;
}

bool CommandEval::parseEndDesc(const QString& str) {

	if (str.isEmpty()) {
		return TRUE;
	}

	if (str.startsWith("Exits:")) {
		roomFlags = FLAGS_VALID;

		const char* data = str.ascii();
		bool doors=FALSE;
		bool road=FALSE;

		for (uint i=7; i<str.length(); i++){
			switch ((int)(data[i])){
		  case 40: doors=TRUE;break;		// (   
		  case 91: doors=TRUE;break;		// [
		  case 61: road=TRUE;break;	   	// =

		  case 110:  // n
			  i+=5;
			  if (doors){
				  SET(roomFlags,DOOR_N);
				  doors=FALSE;
			  }else 
				  SET(roomFlags,EXIT_N);
			  if (road){
				  SET(roomFlags,ROAD_N);
				  road=FALSE;
			  }		    
			  break;

		  case 115:  // s
			  i+=5;
			  if (doors){
				  SET(roomFlags,DOOR_S);
				  doors=FALSE;
			  }else 
				  SET(roomFlags,EXIT_S);
			  if (road){
				  SET(roomFlags,ROAD_S);
				  road=FALSE;
			  }		    
			  break;

		  case 101:  // e
			  i+=4;
			  if (doors){
				  SET(roomFlags,DOOR_E);
				  doors=FALSE;
			  }else 
				  SET(roomFlags,EXIT_E);
			  if (road){
				  SET(roomFlags,ROAD_E);
				  road=FALSE;
			  }		    
			  break;

		  case 119:  // w
			  i+=4;
			  if (doors){
				  SET(roomFlags,DOOR_W);
				  doors=FALSE;
			  }else 
				  SET(roomFlags,EXIT_W);
			  if (road){
				  SET(roomFlags,ROAD_W);
				  road=FALSE;
			  }		    
			  break;

		  case 117:  // u
			  i+=2;
			  if (doors){
				  SET(roomFlags,DOOR_U);
				  doors=FALSE;
			  }else 
				  SET(roomFlags,EXIT_U);
			  if (road){
				  SET(roomFlags,ROAD_U);
				  road=FALSE;
			  }		    
			  break;

		  case 100:  // d
			  i+=4;
			  if (doors){
				  SET(roomFlags,DOOR_D);
				  doors=FALSE;
			  }else 
				  SET(roomFlags,EXIT_D);
			  if (road){
				  SET(roomFlags,ROAD_D);
				  road=FALSE;
			  }		    
			  break;
		  default:;
			}
		}

		return TRUE;
	}
	return FALSE;
}


QString CommandEval::removeAnsiMarks(QString& str) {

	QString out="";
	bool started=FALSE;
	const char* data=str.ascii();

	for (uint i=0; i<str.length(); i++){
		if (data[i]=='m' && started){
			started = FALSE;
			continue;
		}
		if (data[i]==ESC){
			started = TRUE;
			continue;
		}
		if (started) continue;
		out.append(data[i]);
	}
	return out;
}


void CommandEval::promptArrived(QString& prompt){
	if (descriptionready){
		promptFlags = 0;
		index = 0;
		int sv;
		prompt = prompt.stripWhiteSpace();

		switch ((sv=(int)((QChar)(prompt[index])))){
	  case 42: index++;promptFlags=SUN_ROOM;break; // *  // sunlight
	  case 33: index++;break; // !  // artifical light
	  case 41: index++;break; // )  // moonlight
	  case 111:index++;break; // o  // darkness
	  default:;
		}
		/*
		switch ((sv=(int)((QChar)(prompt[index])))){
		case 64: SET(promptFlags,TINDOORS); break; // @  // indoors
		case 35: SET(promptFlags,TCITY); break; // #  // city
		case 95: SET(promptFlags,TFIELD); break; // _  // field
		case 37: SET(promptFlags,TFOREST); break; // %  // forest
		case 40: SET(promptFlags,THILLS); break; // (  // hills
		case 60: SET(promptFlags,TMOUNTAINS); break; // <  // mountains
		case 126:SET(promptFlags,TSHALLOW); break; // ~  // shallow
		case 119:SET(promptFlags,TWATTER); break; // w  // water
		case 87: SET(promptFlags,TRAPIDS); break; // W  // rapids
		case 85: SET(promptFlags,TUNDERWATTER); break; // U  // underwater
		case 43: SET(promptFlags,TROAD); break; // +  // road
		case 61: SET(promptFlags,TTUNNEL); break; // =  // tunnel
		case 79: SET(promptFlags,TCAVERN); break; // O  // cavern
		case 34: SET(promptFlags,TBRUSH); break; // "  // brush
		default:;
		}
		*/
		switch ((sv=(int)((QChar)(prompt[index])))){
	  case 91: SET(promptFlags,TINDOORS); break; // [  // indoors
	  case 35: SET(promptFlags,TCITY); break; // #  // city
	  case 46: SET(promptFlags,TFIELD); break; // .  // field
	  case 102: SET(promptFlags,TFOREST); break; // f  // forest
	  case 40: SET(promptFlags,THILLS); break; // (  // hills
	  case 60: SET(promptFlags,TMOUNTAINS); break; // <  // mountains
	  case 37:SET(promptFlags,TSHALLOW); break; // %  // shallow
	  case 126/*119*/:SET(promptFlags,TWATER); break; // ~/*w*/  // water
	  case 87: SET(promptFlags,TRAPIDS); break; // W  // rapids
	  case 85: SET(promptFlags,TUNDERWATER); break; // U  // underwater
	  case 43: SET(promptFlags,TROAD); break; // +  // road
	  case 61: SET(promptFlags,TTUNNEL); break; // =  // tunnel
	  case 79: SET(promptFlags,TCAVERN); break; // O  // cavern
	  case 58: SET(promptFlags,TBRUSH); break; // :  // brush
	  default:;
		}
		SET(promptFlags,PROMPT_INFO_VALID);

		descriptionready = FALSE;
		Command* c = queue.dequeue();
		if ( !c || (c && c->id!=id_scout)){
			emit characterMoved(c, roomName, roomDesc, parsedRoomDesc, roomFlags, promptFlags);
			emit showPath(queue);
		}
		if (c) delete c;    
	}
}

QString CommandEval::parseServerInput(QString& input,bool descriptionline) {

	Command* c;
	QString name;

	QString str = input.simplifyWhiteSpace();

	//emit sendToClient("[MMapper] :"+str+"\n");

	if (readingRoomDesc) {
		if (parseEndDesc(str)) {  // standard end of description parsed
			readingRoomDesc=FALSE; // we finished read desc mode
			descriptionready = TRUE;

			/*if (following) {
			c=queue.dequeue();
			if ( !c || (c && c->id!=id_scout)){
			emit characterMoved(c, roomName, roomDesc, parsedRoomDesc, 0, 0);
			emit showPath(queue);
			}
			if (c) delete c;	  
			descriptionready = FALSE;
			}*/

			return input;
		} else { // reading room description line
			roomDesc+=(str+"\n");
			if (descriptionline)
				parsedRoomDesc+=(str+"\n");
		}
	} else { //!readingRoomDesc



		//READING BETWEEN Exits: and Prompt (track for example)
		if (!readingRoomDesc && descriptionready) {	  
			return input;
		}

		if ( !((name=parseRoomName(str)).isEmpty()) ) { //Room name arrived
			readingRoomDesc=TRUE; //start of read desc mode
			roomName=name;
			roomDesc="";
			parsedRoomDesc="";
			roomFlags = 0;

		} else
			if (!str.isEmpty() && nonStandardDesc(str)) { // non standard end of description parsed (fog, dark or so ...)
				c=queue.dequeue();
				if ( !c || (c && c->id!=id_scout)){
					emit characterMoved(c,"","","",0,0);
					emit showPath(queue);
				}
				if (c) delete c;
				return input;
			} else { // parse standard input (answers) from server
				str=removeAnsiMarks(str);
				parseAnswers(str);
			}
	}
	return input;
}

void CommandEval::emptyQueue() {
	queue.setAutoDelete(TRUE);
	queue.clear();
	queue.setAutoDelete(FALSE);
}

