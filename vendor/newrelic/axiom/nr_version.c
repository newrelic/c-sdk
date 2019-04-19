#include "nr_axiom.h"
#include "nr_version.h"

/*
 * NR_VERSION ultimately comes from the top-level VERSION file.
 */
#ifndef NR_VERSION
#define NR_VERSION "unreleased"
#endif

/*
 * NR_COMMIT ultimately comes from the command $(git rev-parse HEAD)
 */
#ifndef NR_COMMIT
#define NR_COMMIT ""
#endif

/*
 * The Release code name, assigned by whimsy, referring to a dinosaur,
 * progressing alphabetically.
 * See http://en.wikipedia.org/wiki/List_of_dinosaurs
 *
 * Please update this comment with the name you choose
 * and the date you chose it.
 *
 *   hadrosaurus             05Jul2013 (3.7)
 *   iguanodon               12Jul2013 (3.8)
 *   juratyrant              13Aug2013 (3.9)
 *   khaan                   03Sep2013 (4.0)
 *   lexovisaurus            14Oct2013 (4.1)
 *   micropachycephalosaurus 07Nov2013 (4.2)
 *   nqwebasaurus            25Nov2013 (4.3)  (Adam)
 *   ozraptor                29Dec2013 (4.4)  (Adam)
 *   pyroraptor              31Jan2014 (4.5)  (Mike)
 *   quetzalcoatlus          24Feb2014 (4.6)  (Robert's niece)
 *   richardoestesia         24Feb2014 (4.7)  (Richard England)
 *   spinops                 21Apr2014 (4.8)  (Rich)
 *   trex                    15May2014 (4.9)  (everyone)
 *   unenlagia               18May2014 (4.10) (Robert)
 *   vandersaur              18Jun2014 (4.11) (Rich)
 *   wannanosaurus           23Jul2014 (4.12) (Robert)
 *   xiaotingia              29Aug2014 (4.13) (Aaron)
 *   yulong                  10Sep2014 (4.14) (Adam)
 *   zanabazar               22Sep2014 (4.15) (Rich)
 *
 * On 22Oct2014 we switched to the naming scheme based on bird genera.
 * Here's an exhaustive list:
 *   http://en.wikipedia.org/wiki/List_of_bird_genera
 *
 * There's no wikipedia enumeration of just North America birds,
 * but these lists may prove useful:
 *   https://www.pwrc.usgs.gov/library/bna/bnatbl.htm
 *   http://checklist.aou.org/taxa/
 *
 *   aquila                  22Oct2014 (4.16) (Robert named this one)
 *   barnardius              25Nov2014 (4.17) (Adam named this one)
 *   corvid                  17Dec2014 (4.18) (Walden 2nd Robert's suggestion)
 *   drepanis                20Jan2015 (4.19) (Will named this one)
 *   emberiza                26Feb2015 (4.20) (Adam named this one)
 *   fregata                 27Mar2015 (4.21) (Galen named this one)
 *   gallus                  23Apr2015 (4.22) (Adam named this one)
 *   hydrobatidae            27May2015 (4.23) (Robert named this one)
 *   ispidina                01Jul2015 (4.24) (Erika named this one)
 *   jacana                  14Oct2015 (5.0)  (Will named this one)
 *
 * For release 5.1, we switched to a naming scheme based on women
 * mathematicians. Feel free to use either the first or last name. Here's a list
 * to use: https://en.wikipedia.org/wiki/List_of_women_in_mathematics
 *
 *   ada                     28Oct2015 (5.1) (Chris named this one)
 *   billey                  14Dec2015 (5.2) (Mike named this one)
 *   chudnovsky              19Jan2016 (5.3) (Rich named this one)
 *   driscoll                26Jan2016 (5.4) (Adam named this one)
 *   easley                  08Feb2016 (5.5) (Erika named this one)
 *   freitag                 24Feb2016 (6.0) (Tyler named this one)
 *   gordon                  14Mar2016 (6.1) (Will named this one)
 *   harizanov               22Mar2016 (6.2) (Will named this one)
 *   iyengar                 11Apr2016 (6.3) (Tyler, Rich, and Erika)
 *   jitomirskaya            11May2016 (6.4) (Tyler)
 *   krieger                 22Jun2016 (6.5) (Tyler)
 *   lacampagne              25Jul2016 (6.6) (Mike)
 *   maslennikova            23Aug2016 (6.7) (Aidan)
 *   noether                 20Sep2016 (6.8) (Erika)
 *   owens                   13Dec2016 (6.9) (Rich)
 *   pipher                  12Jan2017 (7.0) (Erika)
 *   roth                    14Feb2017 (7.1) (Adam)
 *   senechal                15Mar2017 (7.2) (Chris)
 *   tjoetta                 19Apr2017 (7.3) (Rich)
 *   uhlenbeck               26Jun2017 (7.4) (Erika)
 *   vaughan                 05Jul2017 (7.5) (Tanya)
 *   weber                   06Sep2017 (7.6) (Erika)
 *   yershova                20Nov2017 (7.7) (Tanya)
 *   zahedi                  25Jan2018 (8.0) (Tanya)
 *
 * For release 8.1, we moved to color names that are also food.
 * Here's a list:
 *  https://en.wikipedia.org/wiki/List_of_colors_(compact)
 *
 *   avocado                 12Mar2018 (8.1) (Tanya)
 *   bisque                  30Apr2018 (8.2) (Tanya)
 *   carrot                  05Sep2018 (8.3) (Alan)
 *   dill                    17Oct2018 (8.4) (Tanya and Amanda)
 *   eggplant                01Dec2018 (8.5) (Alan)
 *   flax                    21Dec2018 (8.6) (Alan)
 *   gin                     29Mar2019 (8.7) (Johannes)
 */
#define NR_CODENAME "gin"

const char* nr_version(void) {
  return NR_STR2(NR_VERSION);
}

const char* nr_version_verbose(void) {
  return NR_STR2(NR_VERSION) " (\"" NR_CODENAME
                             "\" - \"" NR_STR2(NR_COMMIT) "\")";
}
