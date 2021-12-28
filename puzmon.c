/*=== puzmon4: ソースコード雛形 ===*/
/*** インクルード宣言 ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include <math.h>


/*** 列挙型宣言 ***/
typedef enum{FIRE, WATER, WIND, EARTH, LIFE, EMPTY} Element;
enum{MAX_GEMS=14};
enum{ELEM_NUM=6};


/*** グローバル定数の宣言 ***/
const char ELEMENT_SYMBOLS[6] = {'$', '~', '@', '#', '&', ' '};
const int ELEMENT_COLORS[6] = {1, 6, 2, 3, 5, 0};
const double ELEMENT_BOOST[4][4] = {
    {1.0, 0.5, 2.0, 1.0},
    {2.0, 1.0, 1.0, 0.5},
    {0.5, 1.0, 1.0, 2.0},
    {1.0, 2.0, 0.5, 1.0}
};

/*--
　　火　水　風　土（列 敵のパラメータを渡す）
    火　１　0.5 2  1
    水　2  1   1  0.5
風　0.5 1  1  2
土　1  2  0.5  1
（行　見方のパラメータを渡す）
--*/


/*** 構造型宣言 ***/
typedef struct MONSTER{
    char* name;
    int hp;
    int maxHp;
    int type;
    int attack;
    int diffence;
} Monster;

typedef struct DUNGEON{
    Monster *ememies;
    int roop;
} Dungeon;

typedef struct PARTY{
    char* playerName;
    Monster* myMonsters;
    int monsterNum;
    int hp;
    int maxHp;
    int diffence;
} Party;

typedef struct BATTLE_FIELD
{
    Party* party;
    Monster* enemy;
    Element gems[MAX_GEMS];
} BattleField;


typedef struct BANISHINFO {
    Element type;
    int start;
    int contNum;
} BanishInfo;



/*** プロトタイプ宣言 ***/
void goDungeon(Party *Party);
void doBattle(Monster *eEmemy, Party *pParty);
void printMonsterName(Monster *m);
Party organizeParty(char *playerName, Monster *ms, int num);
void showParty(Party *pParty);
void onPlayerTurn(BattleField* bBattleField);
void onEnemyTurn(BattleField* bBattlefield);
void doAttack(BattleField* bBattleField, BanishInfo* pB);
void doEnemyAttack(BattleField* pField);
void fillGems(Element* gems);
void printGems(Element* gems);
void printGem(Element gem);
void showbattleField(BattleField* bBattlefield);
bool checkValidCommand(char* command);
void evaluateGems(BattleField* bBattleField);
void moveGem(Element* gems, int c1, int c2, bool isProcess);
void swapGem(Element* gems, int pos, int step);
BanishInfo checkBanishable(Element* gems);
void banishGems(BattleField* pField, BanishInfo* pBanishable);
void shiftGems(Element* gems);
void spawngems(Element* gems);
int countGems(Element* gems, Element gem);
void doRecover(BattleField* bBattlefield, BanishInfo* pBanish);
double calcAttackDamage(BattleField* pField, BanishInfo* pB);
double calcrecoverDamage(BanishInfo* pB);
double blurDamage();
double calcEnemyAttackDamage(BattleField* pField);


/*** 関数宣言 ***/
void goDungeon(Party *pParty){
    printf("%sのパーティ(HP=%d)はダンジョンに到着した\n", pParty->playerName, pParty->hp);
    showParty(pParty);

    //ダンジョンのモンスターを定義
    Monster srime = {"スライム", 100, 100, WATER, 10, 5};
    Monster goburin = {"ゴブリン", 200, 200, EARTH, 20, 15};
    Monster ookoumori = {"オオコウモリ", 300, 300, WIND, 30, 25};
    Monster weauruhu = {"ウェアウルフ", 400, 400, WIND, 40, 30};
    Monster dragon = {"ドラゴン", 800, 800, FIRE, 50, 40};
    Monster dungeon_monsters[] = {srime, goburin, ookoumori, weauruhu, dragon};
    int ENEMYNUM = sizeof(dungeon_monsters) / sizeof(Monster);

    // ダンジョンの構造体を生成
    Dungeon dungeon1 = {&dungeon_monsters[0], ENEMYNUM};

    // モンスター一体ずつと戦闘を開始
    for (int i = 0; i < dungeon1.roop; i++)
    {
        doBattle(&dungeon1.ememies[i], pParty);
    }
    printf("%sはダンジョンを制覇した！\n", pParty->playerName);
}


void doBattle(Monster *eEnemy, Party *pParty)
{
    printf("\n");
    printMonsterName(eEnemy);
    printf("が現れた！\n\n");
    BattleField battlefield = {pParty, eEnemy};
    fillGems(battlefield.gems); // 引数は配列型

    //自分か相手のターンを繰り返す
    while (pParty->hp > 0 || eEnemy->hp > 0){
        onPlayerTurn(&battlefield);
        if (eEnemy->hp <= 0){
            printMonsterName(eEnemy);
            printf("を倒した！\n");
            printf("%sはさらに奥へと進んだ\n\n", pParty->playerName);
            break;
        }
        onEnemyTurn(&battlefield);
        if (pParty->hp <= 0){
            break;
        }
    }
    printf("===================\n\n");
}


void showParty(Party *pParty)
{
    printf("＜パーティ編成＞----------\n");
    for (int i = 0; i < pParty->monsterNum; i++){
        Monster im = pParty->myMonsters[i];
        printMonsterName(&im);
        printf(" HP= %d 攻撃= %d 防御= %d\n", im.hp, im.attack, im.diffence);
    }
    printf("-------------------------\n");
}


Party organizeParty(char *playerName, Monster *myMonsters, int num){
    Party party = {playerName, myMonsters, num, 0, 0, 0};
    for (int i = 0; i < num; i++){
        party.hp += myMonsters[i].hp;
        party.maxHp += myMonsters[i].maxHp;
        party.diffence += myMonsters[i].diffence;
    }
    party.diffence /= num;
    return party;
}

void onPlayerTurn(BattleField* bBattleField){
    Party* pParty = bBattleField->party;
    Monster* eEnemy = bBattleField->enemy;
    printf("【%sのターン】\n", pParty->playerName);
    showbattleField(bBattleField);

    //2文字のコマンドを入力する
    char command[2];
    bool isValid = false;
    do{
        printf("コマンド？>");
        scanf("%s", command);
        bool isValid = checkValidCommand(command);
    } while(isValid);
    printGems(bBattleField->gems);
    moveGem(bBattleField->gems, command[0]-'A', command[1] - 'A', true);
    evaluateGems(bBattleField);
}

void onEnemyTurn(BattleField* bBattlefield){
    Party* pParty = bBattlefield->party;
    Monster* eEnemy = bBattlefield->enemy;
    printf("【");
    printMonsterName(eEnemy);
    printf("のターン】\n");
    doEnemyAttack(bBattlefield);
}

void doAttack(BattleField* pField, BanishInfo* pB){
    double damage;
    damage = calcAttackDamage(pField, pB);
    pField->enemy->hp -= damage;
    printf("%sに%fのダメージ！\n\n", pField->enemy->name, damage);
}

void doRecover(BattleField* pField, BanishInfo* pBanish){
    double recover;
    recover = calcrecoverDamage(pBanish);
    pField->party->hp += recover;
    if (pField->party->hp > pField->party->maxHp){
        pField->party->hp = pField->party->maxHp;
    }
    printf("%f回復した!\n\n", recover);
}

void doEnemyAttack(BattleField* pField){
    double damage = calcEnemyAttackDamage(pField);
    pField->party->hp -= damage;
    printf("%dのダメージを受けた！\n\n", damage);
}


void showbattleField(BattleField* bBattlefield){
    Party* pParty = bBattlefield->party;
    Monster* eEnemy = bBattlefield->enemy;
    printf("-------------------------\n\n\n");
    printMonsterName(eEnemy);
    printf("\nHP=%d/%d\n\n\n\n", eEnemy->hp, eEnemy->maxHp);
    for (int i=0; i<pParty->monsterNum; i++){
        printMonsterName(&(pParty->myMonsters[i]));
        printf(" ");
    }
    printf("\nHP=%d/%d\n\n", pParty->hp, pParty->maxHp);
    printf("-------------------------\n\n");
    printf("A B C D E F G H I J K L M N\n");
    printGems(bBattlefield->gems);
    printf("\n-------------------------\n");
}


bool checkValidCommand(char* command){
    if (strlen(command) != 2){
        return false;
    }
    if (command[0] == command[1]){
        return false;
    }
    if  (command[0] < 'A' || command[0] > MAX_GEMS - 1){
        return false;
    }
    if  (command[1] < 'A' || command[1] > MAX_GEMS - 1){
        return false;
    }
    return true;
}


void evaluateGems(BattleField* bBattleField){
    BanishInfo pB = checkBanishable(bBattleField->gems);
    if (pB.contNum != 0){
        banishGems(bBattleField, &pB);
        shiftGems(bBattleField->gems);
        spawngems(bBattleField->gems);
    }
}


BanishInfo checkBanishable(Element* gems){
    for (int i=0; i<MAX_GEMS-2; i++){
        int len = 1;
        if (gems[i] != EMPTY){
            for (int j=i+1; j<MAX_GEMS; j++){
                if(gems[i] == gems[j]){
                    len++;
                }else{
                     break;
                }
            }
        }

        if (len >= 3){
            BanishInfo found = {gems[i], i, len}; //BanishInfo = {Element（連続している要素）, int(start地点), int(連続数)}
            return found;
        }
    }
    BanishInfo notFound = {EMPTY, 0, 0};
    return notFound;
}


void banishGems(BattleField* pField, BanishInfo* pB){
    int pos = pB->start;
    for (int i=pos; i<pos+pB->contNum; i++){
        (pField->gems)[i] = EMPTY;
    }
    printGems(pField->gems);
    if (pB->type != LIFE){
        doAttack(pField, pB);
    } else {
        doRecover(pField, pB);
    }
}


void shiftGems(Element* gems){
    int count = countGems(gems, EMPTY);
    for (int i=0; i<MAX_GEMS-count; i++){
        if(gems[i] == EMPTY){
            moveGem(gems, i, MAX_GEMS-1, false);
            printGems(gems);
            i--;
        }
    }
}


void spawngems(Element* gems){
    for (int i=0; i<MAX_GEMS; i++){
        if (gems[i] == EMPTY){
            gems[i] = rand() % (LIFE+1);
        }
    }
    printGems(gems);
}




// メイン関数
int main(int argc, char **argv){
    srand((unsigned)time(0UL));
    if (argc != 2){
        printf("プレイヤー名を指定してゲームを開始してください\n");
        return 1;
    }

    Monster suzaku = {"朱雀", 150, 150, FIRE, 25, 10};
    Monster seiryu = {"青龍", 150, 150, WIND, 15, 10};
    Monster byakko = {"白虎", 150, 150, EARTH, 20, 5};
    Monster genbu = {"玄武", 150, 150, WATER, 20, 15};
    Monster myMonsters[] = {suzaku, seiryu, byakko, genbu};
    int MYPARYNUM = sizeof(myMonsters) / sizeof(Monster);

    /* GAME START */
    printf("***Puzzle & Monsters ***\n");
    char *playerName = argv[1];
    Party party = organizeParty(playerName, myMonsters, MYPARYNUM);
    goDungeon(&party);
    printf("*** GAME CLEARED! ***\n");
    printf("倒したモンスター数：5");
    return 0;
}



/*** ユーティリティ関数宣言 ***/
void printMonsterName(Monster* m){
    int color = ELEMENT_COLORS[m->type];
    char symbol = ELEMENT_SYMBOLS[m->type];
    printf("\x1b[3%dm", color);
    printf("%c%s%c", symbol, m->name, symbol);
    printf("\x1b[39m");
}


void fillGems(Element* gems){
    for (int i=0; i < MAX_GEMS; i++){
        gems[i] = rand() % ELEM_NUM;
    }
}

void printGems(Element* gems){
    for (int i=0; i<MAX_GEMS; i++){
        printGem(gems[i]);
    }
    printf("\n");
}

void printGem(Element gem){
    int color = ELEMENT_COLORS[gem];
    int symbol = ELEMENT_SYMBOLS[gem];
    printf("\x1b[4%dm", color);
    printf("\x1b[30m");
    printf("%c", symbol);
    printf("\x1b[39m");
    printf("\x1b[49m");
    printf(" ");
}

void moveGem(Element* gems, int c1, int c2, bool isProcess){
    int step = (c2>c1) ? 1:-1;
    for (int i=c1; i!=c2; i+= step){
        swapGem(gems, i, step);
        if (isProcess) printGems(gems);
    }
}


void swapGem(Element* gems, int pos, int step){
    Element buf = gems[pos];
    gems[pos] = gems[pos+step];
    gems[pos+step] = buf;
}


int countGems(Element* gems, Element gem){
    int result = 0;
    for (int i=0; i<MAX_GEMS; i++){
        if (gems[i] == gem){
            result++;
        }
    }
    return result;
}


double calcAttackDamage(BattleField* pField, BanishInfo* pB){
    double result;
    const int CONBO = 0;
    Party* myParty = pField->party;
    Monster* myMonsters = myParty->myMonsters;
    for (int i=0; i<myParty->monsterNum; i++){
        if (myMonsters[i].type == pB->type){
            result = (myMonsters[i].attack - pField->enemy->diffence) * ELEMENT_BOOST[myMonsters[i].type][pField->enemy->type] * pow(1.5, (pB->contNum-3 + CONBO + 2)) * blurDamage();
            return result;
        }
    }
    // （攻撃モンスターの攻撃力 - 敵の防御力）* 属性補正 * (1.5 ^ (消滅宝石数-3＋コンボ数))　±10%
    // コンボは0で考える
    // 攻撃モンスターのポインタ、敵モンスターのポインタ、banishinfoのポインタを引数に取れば良さげ
}


double calcrecoverDamage(BanishInfo* pB){
    double result;
    int COMBO = 0;
    result = 20 * pow(1.5, pB->contNum-3 + COMBO) * blurDamage();
    return result;
}

double blurDamage(){
    double randDamage = 9 + rand() % 2;
    randDamage = randDamage / 10;
    return randDamage;
}


double calcEnemyAttackDamage(BattleField* pField){
    Monster* eEnemy = pField->enemy;
    Party* pParty = pField->party;
    double result;
    result = (eEnemy->attack - pParty->diffence) * blurDamage();
    return result;
    // (敵モンスターの攻撃力-パーティ防御力)±10%
    // 値の変動はblurDamage関数で0.9-1.1の値を返す
} 
