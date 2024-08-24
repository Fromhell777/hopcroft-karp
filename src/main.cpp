#include "timer.hpp"
#include "prettyprint.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <bitset>

bool breadth_first_search(
  std::vector<std::vector<std::size_t>> const & cocktail_to_barmen,
  std::vector<std::size_t> const & matches_order,
  std::vector<std::size_t> const & matches_barmen,
  std::vector<std::size_t> & distance_order)
{
  std::queue<std::size_t> cocktail_queue;

  for (std::size_t i = 0; i < matches_order.size(); ++i)
    {
      if (matches_order[i] == matches_barmen.size())
        {
          distance_order[i] = 0;
          cocktail_queue.emplace(i);
        }
      else
        {
          distance_order[i] = matches_barmen.size();
        }
    }

  while (not cocktail_queue.empty())
    {
      auto const cocktail = cocktail_queue.front();
      cocktail_queue.pop();

      for (auto const barman : cocktail_to_barmen[cocktail])
        {
          if (matches_barmen[barman] == matches_order.size())
            {
              return true;
            }
          else if (distance_order[matches_barmen[barman]] == matches_barmen.size())
            {
              distance_order[matches_barmen[barman]] = distance_order[cocktail] + 1;
              cocktail_queue.emplace(matches_barmen[barman]);
            }
        }
    }

  return false;
}

bool depth_first_search(
  std::size_t const cocktail,
  std::vector<std::vector<std::size_t>> const & cocktail_to_barmen,
  std::vector<std::size_t> & matches_order,
  std::vector<std::size_t> & matches_barmen,
  std::vector<std::size_t> & distance_order)
{
  for (auto const barman : cocktail_to_barmen[cocktail])
    {
      if (matches_barmen[barman] == matches_order.size())
        {
          matches_barmen[barman] = cocktail;
          matches_order[cocktail] = barman;
          return true;
        }
      else if (distance_order[matches_barmen[barman]] ==
               distance_order[cocktail] + 1)
        {
          if (depth_first_search(matches_barmen[barman], cocktail_to_barmen,
                                 matches_order, matches_barmen, distance_order))
            {
              matches_barmen[barman] = cocktail;
              matches_order[cocktail] = barman;
              return true;
            }
        }
    }

  distance_order[cocktail] = matches_barmen.size();
  return false;
}

std::string find_order_validity(std::vector<std::bitset<26>> const & recipes,
                                std::string const & order)
{
  std::vector<std::vector<std::size_t>> cocktail_to_barmen(order.size());

  for (std::size_t i = 0; i < order.size(); ++i)
    {
      for (std::size_t j = 0; j < recipes.size(); ++j)
        {
          if (recipes[j][order[i] - 65])
            {
              cocktail_to_barmen[i].emplace_back(j);
            }
        }

      // A cocktail cannot be made by any barmen
      if (cocktail_to_barmen[i].empty())
        {
          return "onmogelijk";
        }
    }

  // Perform the Hopcroft-Karp algorithm
  std::vector<std::size_t> matches_order(order.size(), recipes.size());

  std::vector<std::size_t> matches_barmen(recipes.size(), order.size());

  std::size_t maximum_cardinality_matching = 0;

  std::vector<std::size_t> distance_order(order.size());

  while (breadth_first_search(cocktail_to_barmen, matches_order,
                              matches_barmen, distance_order))
    {
      for (std::size_t i = 0; i < order.size(); ++i)
        {
          if (matches_order[i] == recipes.size())
            {
              if (depth_first_search(i, cocktail_to_barmen, matches_order,
                                     matches_barmen, distance_order))
                {
                  ++maximum_cardinality_matching;
                }
            }
        }
    }

  if (maximum_cardinality_matching == order.size()) {return "mogelijk";}
  else {return "onmogelijk";}
}

int main()
{
  {
    timer timer;

    std::size_t test_cases;
    std::cin >> test_cases;

    for (std::size_t t = 0; t < test_cases; ++t)
      {
        std::size_t number_of_barmen;
        std::cin >> number_of_barmen;

        std::string dummy;
        std::getline(std::cin, dummy);

        std::vector<std::bitset<26>> recipes(number_of_barmen);

        for (std::size_t i = 0; i < number_of_barmen; ++i)
          {
            std::string recipe;
            std::getline(std::cin, recipe);

            for (auto const cocktail : recipe)
              {
                recipes[i].set(cocktail - 65);
              }
          }

        std::size_t number_of_orders;
        std::cin >> number_of_orders;

        std::getline(std::cin, dummy);

        for (std::size_t i = 0; i < number_of_orders; ++i)
          {
            std::string order;
            std::getline(std::cin, order);

            std::cout << order << ' ' << find_order_validity(recipes, order)
                      << '\n';
          }

      }
  }
}
