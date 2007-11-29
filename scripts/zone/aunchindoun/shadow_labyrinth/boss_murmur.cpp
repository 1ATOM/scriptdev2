/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ScriptData
SDName: Boss_Murmur
SD%Complete: 100
SDComment: 
EndScriptData */

#include "../../../sc_defines.h"

#define SPELL_SONIC_BOOM           33666
#define SPELL_SONIC_BOOM_CAST      38796
#define SPELL_MURMURS_TOUCH        38794
#define SPELL_RESONANCE            33657

struct MANGOS_DLL_DECL boss_murmurAI : public ScriptedAI
{
    boss_murmurAI(Creature *c) : ScriptedAI(c) {EnterEvadeMode();}

    uint32 SonicBoom_Timer;
    uint32 MurmursTouch_Timer;
    uint32 Resonance_Timer;
    bool SonicBoomVisual;

    bool InCombat;

    void EnterEvadeMode()
    {
        SonicBoom_Timer = 30000;
        MurmursTouch_Timer = 20000;
        Resonance_Timer = 10000;
        SonicBoomVisual = false;

        InCombat = false;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop();
        DoGoHome();
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true); 
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CONFUSED, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM , true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR , true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DAZE, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
    }

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            //Begin melee attack if we are within range
            DoStartMeleeAttack(who);
            InCombat = true;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && m_creature->IsWithinLOSInMap(who))
            {
                if(who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                DoStartMeleeAttack(who);
                InCombat = true;

            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

        //SonicBoom_Timer
        if(SonicBoom_Timer < diff)
        {
            if(!SonicBoomVisual)
            {
                DoCast(m_creature, SPELL_SONIC_BOOM_CAST);
                SonicBoomVisual = true;
                SonicBoom_Timer = 5000;
            }
            else
            {
                // it damage only the main tank, and it don't divides damage between other members / core related
                //DoCast(m_creature->getVictim(), SPELL_SONIC_BOOM);
                SonicBoomVisual = false;
                SonicBoom_Timer = 30000;
            }
        }else SonicBoom_Timer -= diff;

        //MurmursTouch_Timer
        if(MurmursTouch_Timer < diff)
        {
            Unit* target = NULL;
            target = SelectUnit(SELECT_TARGET_RANDOM,0);
            if(target)
                DoCast(target, SPELL_MURMURS_TOUCH);

            MurmursTouch_Timer = 30000;
        }else MurmursTouch_Timer -= diff;

        //Resonance_Timer
        if(Resonance_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_RESONANCE);

            Resonance_Timer = 40000;
        }else Resonance_Timer -= diff;

        DoMeleeAttackIfReady();
    }
}; 
CreatureAI* GetAI_boss_murmur(Creature *_Creature)
{
    return new boss_murmurAI (_Creature);
}

void AddSC_boss_murmur()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_murmur";
    newscript->GetAI = GetAI_boss_murmur;
    m_scripts[nrscripts++] = newscript;
}